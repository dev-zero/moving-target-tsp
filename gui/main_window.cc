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
    _targetsSelected(0)
{
    _ui = new Ui::MainWindow;
    _ui->setupUi(this);

    connect(_ui->datafileBrowse, SIGNAL(clicked()), this, SLOT(_datafileOpen()));
    connect(_ui->datafileLoad, SIGNAL(clicked()), this, SLOT(_datafileLoad()));

    _targetsModel = new QStandardItemModel;
    _ui->targetsList->setModel(_targetsModel);

    connect(_ui->targetsList->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(_targetSelectionChanged(const QItemSelection&, const QItemSelection&)));

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
    QString filename(QFileDialog::getOpenFileName(this));
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
    _targetsTotal = _targetsSelected = 0;
    _updateTargetNumbers();
}

void MainWindow::parserThreadFinished()
{
    logToConsole("parsing finished");
    _ui->datafileLoad->setEnabled(true);
    _updateTargetNumbers();
}

void MainWindow::addTarget(const QString& name)
{
    _targetsModel->appendRow(new QStandardItem(name));
    ++_targetsTotal;
}

void MainWindow::_updateTargetNumbers()
{
    _ui->targetsSelected->setText(QLocale().toString(_targetsSelected));
    _ui->targetsTotal->setText(QLocale().toString(_targetsTotal));
}

void MainWindow::_targetSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    _targetsSelected += selected.indexes().size() - deselected.indexes().size();
    _updateTargetNumbers();
}
