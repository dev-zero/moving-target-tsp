/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include "main_window.hh"

#include <QFileDialog>
#include <QStatusBar>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QLocale>

// auto-generated
#include "ui_simple.h"


MainWindow::MainWindow() :
    QMainWindow(),
    _targetsTotal(0),
    _targetsSelected(0),
    _computationRunning(false)
{
    _ui = new Ui::MainWindow;
    _ui->setupUi(this);

    connect(_ui->datafileBrowse, SIGNAL(clicked()), this, SLOT(_datafileOpen()));
    connect(_ui->datafileLoad, SIGNAL(clicked()), this, SLOT(_datafileLoad()));

    _targetsModel = new QStandardItemModel;
    _ui->targetsList->setModel(_targetsModel);

    connect(_ui->targetsList->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(_targetSelectionChanged(const QItemSelection&, const QItemSelection&)));

    connect(_ui->computationCommand, SIGNAL(clicked()), this, SLOT(_computationCommand()));

    _ui->methodWarning->setVisible(false);

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

void MainWindow::_datafileOpen()
{
    QString filter("Comma-separate value list (*.txt *.csv)");
    if (_ui->datafileType->currentText() == "HIP2")
        filter = "Hipparcos-2 data (*.dat)";

    QString filename(
            QFileDialog::getOpenFileName(this,
                tr("Open data file"),
                QString(),
                filter));
    _ui->datafileName->setText(filename); // in case it's empty, placeholder will be displayed
}

void MainWindow::_datafileLoad()
{
    if (_ui->datafileName->text().isEmpty())
    {
        QMessageBox::warning(this, windowTitle(), tr("No file specified"));
        return;
    }
    _ui->datafileLoad->setEnabled(false);
    logToConsole(
            QString("loading file %1 using the %2 parser").
            arg(_ui->datafileName->text()).
            arg(_ui->datafileType->currentText()));
    emit parsingRequested(
            _ui->datafileName->text(),
            _ui->datafileType->currentText());
}

void MainWindow::parserThreadStarted()
{
    logToConsole("parsing started");
    _targetsModel->clear();
    _ui->renderingWidget->clear();
    _targetsTotal = _targetsSelected = 0;
    _updateTargetNumbers();
}

void MainWindow::parserThreadFinished()
{
    logToConsole("parsing finished");
    _ui->datafileLoad->setEnabled(true);
    _updateTargetNumbers();

    if (_ui->datafileType->currentText() == "CSV")
        _ui->methodVelocityUnit->setText("units");
    else if (_ui->datafileType->currentText() == "HIP2")
        _ui->methodVelocityUnit->setText("parsec/year (=~ 9.78*10^8 m/s)");
}

void MainWindow::addTarget(const TargetDataQt& t)
{
    QString line(t.name);

    /* if the additional data contains a description,
     * append it to the name */

    QMap<QString, QVariant>::const_iterator f;

    f = t.data.find("description");
    if (f != t.data.end())
        line += QString(" (%1)").arg(f->toString());

    QStandardItem* item(new QStandardItem(line));
    item->setData(QVariant::fromValue(t), Qt::UserRole + 1);

    unsigned int idx(_ui->renderingWidget->addTarget(t));
    item->setData(QVariant::fromValue(idx), Qt::UserRole + 2); // first role idx is used for TargetDataQt

    _targetsModel->appendRow(item);

    ++_targetsTotal;
}

void MainWindow::_updateTargetNumbers()
{
    _ui->targetsSelected->setText(QLocale().toString(_targetsSelected));
    _ui->targetsTotal->setText(QLocale().toString(_targetsTotal));
}

void MainWindow::_targetSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
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
}

void MainWindow::computationThreadStarted()
{
    logToConsole("computation started");
    _computationRunning = true;
    _ui->computationCommand->setText("Abort computation");
    _ui->datafileSection->setEnabled(false);
    _ui->targetsSection->setEnabled(false);
    _ui->methodSection->setEnabled(false);
}

void MainWindow::computationThreadFinished()
{
    logToConsole("computation finished");
    _computationRunning = false;
    _ui->computationCommand->setText("Start computation");
    _ui->datafileSection->setEnabled(true);
    _ui->targetsSection->setEnabled(true);
    _ui->methodSection->setEnabled(true);
}

void MainWindow::_computationCommand()
{
    if (_computationRunning)
    {
        emit computationStopRequested();
    }
    else
    {
        QList<TargetDataQt> targets;
        QModelIndex idx;
        foreach(idx, _ui->targetsList->selectionModel()->selectedIndexes())
        {
            targets.push_back(idx.data(Qt::UserRole + 1).value<TargetDataQt>());
        }
        emit computationRequested(targets, _ui->methodVelocity->value());
    }
}

void MainWindow::displayPath(const QList<std::array<double,3>>& list)
{
    logToConsole("found a path, drawing...");
    _ui->renderingWidget->displayPath(list);
}
