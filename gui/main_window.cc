/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include "main_window.hh"
#include "gui/target_manager.hh"
#include "gui/computation_manager.hh"
#include "utils/common_calculation_functions.hh"

#include <QFileDialog>
#include <QStatusBar>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QLocale>
#include <QDebug>

// auto-generated
#include "ui_simple.h"
#include "ui_simulated_annealing_cooling_schedule.h"

/*
 * TODOs
 * http://lists.trolltech.com/qt-interest/2008-04/thread00539-0.html
 */

MainWindow::MainWindow() :
    QMainWindow(),
    _computationRunning(false)
{
    _ui = new Ui::MainWindow;
    _ui->setupUi(this);

    _targetsModel = new QStandardItemModel(this);
    QStringList targetsModelHeader;
    targetsModelHeader << "Identifier" << "Position" << "Velocity";
    _targetsModel->setHorizontalHeaderLabels(targetsModelHeader);
    _targetManager = new TargetManager(_targetsModel, this);
    
    _saDialog = new QDialog(this);
    _uiSADialog = new Ui::SimulatedAnnealingDialog;
    _uiSADialog->setupUi(_saDialog);

    _ui->targetsList->setModel(_targetsModel);

    _computationManager = new ComputationManager(this);

    connect(_computationManager, SIGNAL(started()), SLOT(_computationStarted()));
    connect(_computationManager, SIGNAL(finished()), SLOT(_computationFinished()));
    connect(_computationManager, SIGNAL(solutionFound(const QList<std::array<double,3>>&, double, double)), SLOT(_displayPath(const QList<std::array<double,3>>&, double, double)));

    connect(_targetManager, SIGNAL(targetAdded(QStandardItem*)), SLOT(_addTargetObject(QStandardItem*)));
    // TODO: we currently can't remove items since it would change our associated index
    connect(_targetManager, SIGNAL(targetRemoved(unsigned int)), _ui->renderingWidget, SLOT(disableTarget(unsigned int)));
    connect(_targetManager, SIGNAL(allTargetsRemoved()), _ui->renderingWidget, SLOT(clear()));

    connect(_ui->openTargetManager, SIGNAL(clicked()), _targetManager, SLOT(open()));

    connect(_ui->computationCommand, SIGNAL(clicked()), SLOT(_computationCommand()));

    connect(_ui->methodType, SIGNAL(activated(const QString&)), SLOT(_changeMethodType(const QString&)));
    connect(_ui->methodType, SIGNAL(activated(const QString&)), _computationManager, SLOT(switchAlgorithm(const QString&)));
    connect(_ui->methodOptions, SIGNAL(clicked()), SLOT(_showMethodOptionsDialog()));
    connect(_saDialog, SIGNAL(finished(int)), SLOT(_methodOptionsDialogFinished(int)));

    connect(_ui->velocity, SIGNAL(valueChanged(double)), SLOT(_updateVelocityFromUnits(double)));
    connect(_ui->velocityFractions, SIGNAL(valueChanged(double)), SLOT(_updateVelocityFromFractions(double)));

    connect(_targetManager, SIGNAL(numberOfTargetsChanged(int)), SLOT(_updateNumberOfTargets(int)));
    _ui->methodWarning->setVisible(false);

    _ui->targetDetailsDock->setVisible(false);

    statusBar()->showMessage("ready");
}

MainWindow::~MainWindow()
{
//    delete _targetsModel; // Qt should clean it up
    delete _ui;
}

void MainWindow::logToConsole(const QString& text)
{
    _ui->consoleOutput->appendPlainText(text);    
}

void MainWindow::_updateNumberOfTargets(int number)
{
    _ui->targetsTotal->setText(QLocale().toString(number));
    if (number > 0)
        _ui->computationCommand->setEnabled(true);
    else
        _ui->computationCommand->setEnabled(false);
}

void MainWindow::_addTargetObject(QStandardItem* item)
{
    TargetDataQt t(item->data(Qt::UserRole + 1).value<TargetDataQt>());
//    qDebug() << "adding rendering target for target " << t.name;
    unsigned int idx(_ui->renderingWidget->addTarget(t));
    _ui->renderingWidget->enableTarget(idx);
    item->setData(QVariant::fromValue(idx), Qt::UserRole + 2);
}

// some leftovers...
// TODO: the html_content-update is currently missing and needs to be re-integrated
/*
   QModelIndex currentIndex(_ui->targetsList->selectionModel()->currentIndex());
   if (currentIndex.isValid())
   {
   const TargetDataQt& t(_targetsModel->data(currentIndex, Qt::UserRole + 1).value<TargetDataQt>());

// update base data
_ui->targetDetailsName->setText(t.name);
_ui->targetDetailsPosition->setText(QString("%1 / %2 / %3").arg(t.position[0]).arg(t.position[1]).arg(t.position[2]));
_ui->targetDetailsVelocity->setText(QString("%1 / %2 / %3").arg(t.velocity[0]).arg(t.velocity[1]).arg(t.velocity[2]));

if (t.data.size() > 0)
{
QString html_content("<html><title>Additional data</title><body><dl>");
const QString html_entry("<dt>%1</dt><dd>%2</dd>");
for (QMap<QString, QVariant>::const_iterator i(t.data.begin()), i_end(t.data.end()); i != i_end; ++i)
{
html_content += html_entry.arg(i.key()).arg(i.value().toString());
}
html_content += "</dl></body></html>";

_ui->targetDetailsWebView->setHtml(html_content);
}
}

QModelIndex idx;
unsigned int t_idx;
foreach(idx, selected.indexes())
{
t_idx = _targetsModel->data(idx, Qt::UserRole + 2).value<unsigned int>();
_ui->renderingWidget->enableTarget(t_idx);
}
foreach(idx, deselected.indexes())
{
t_idx = _targetsModel->data(idx, Qt::UserRole + 2).value<unsigned int>();
_ui->renderingWidget->disableTarget(t_idx);
}
_targetsSelected += selected.indexes().size() - deselected.indexes().size();
_updateTargetNumbers();

if (_targetsSelected > 0)
_ui->computationCommand->setEnabled(true);
else
_ui->computationCommand->setEnabled(false);

_ui->methodWarning->setVisible(_targetsSelected >= 12);
*/

void MainWindow::_computationStarted()
{
    logToConsole("computation started");
    _computationRunning = true;
    _ui->computationCommand->setText("Abort computation");
    _ui->openTargetManager->setEnabled(false);
    _ui->targetsSection->setEnabled(false);
    _ui->methodSection->setEnabled(false);
}

void MainWindow::_computationFinished()
{
    logToConsole("computation finished");
    _computationRunning = false;
    _ui->computationCommand->setText("Start computation");
    _ui->openTargetManager->setEnabled(true);
    _ui->targetsSection->setEnabled(true);
    _ui->methodSection->setEnabled(true);
}

void MainWindow::_computationCommand()
{
    if (_computationRunning)
        emit _computationManager->abort();
    else
    {
        QList<TargetDataQt> targets;
        for (int i(0); i < _targetsModel->rowCount(); ++i)
            targets.push_back(_targetsModel->item(i)->data(Qt::UserRole + 1).value<TargetDataQt>());

        _computationManager->switchAlgorithm(_ui->methodType->currentText());
        _computationManager->setData(targets, _ui->velocity->value());
        _computationManager->start();
    }
}

void MainWindow::_displayPath(const QList<std::array<double,3>>& list, double time, double length)
{
    logToConsole("found a path, drawing...");
    _ui->renderingWidget->displayPath(list);
    _ui->totalExternalTime->setText(QLocale().toString(time) + " years");
    const double velocity(parsec_per_year2meters_per_second(_ui->velocity->value()));
    const double internalTime(time*sqrt(1.0-((velocity*velocity)/(physical_constants::c*physical_constants::c))));
    _ui->totalInternalTime->setText(QLocale().toString(internalTime) + " years");
    _ui->totalLength->setText(QLocale().toString(length) + " parsec");
    qDebug() << "time:" << time << ", traveller time:" << internalTime << ", total length:" << length;
}

void MainWindow::_changeMethodType(const QString& type)
{
    if (type == "Simulated Annealing")
        _ui->methodOptions->setEnabled(true);
    else
        _ui->methodOptions->setEnabled(false);
}

void MainWindow::_showMethodOptionsDialog()
{
    if (_ui->methodType->currentText() == "Simulated Annealing")
        _saDialog->setVisible(true);
}

void MainWindow::_methodOptionsDialogFinished(int result)
{
    if (result == QDialog::Accepted)
    {
        QMap<QString, QVariant> options;
        options["initialTemperature"]      = _uiSADialog->initialTemperature->value();
        options["finalTemperature"]        = _uiSADialog->finalTemperature->value();
        options["decreaseCoefficient"]     = _uiSADialog->decreaseFactor->value();
        options["maxSameTemperatureSteps"] = _uiSADialog->equalTemperatureSteps->value();
        _computationManager->setParameters("Simulated Annealing", options);
    }
}

void MainWindow::_updateVelocityFromUnits(double v)
{
    _ui->velocityFractions->setValue(parsec_per_year2fractions_of_c(v));
}

void MainWindow::_updateVelocityFromFractions(double v)
{
    _ui->velocity->setValue(fractions_of_c2parsec_per_year(v));
}
