/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef MAIN_WINDOW_HH
#define MAIN_WINDOW_HH

#include <QtGui/QMainWindow>
#include <QtGui/QItemSelection>
#include <array>
#include <tuple>
#include "base/target_data.hh"

/// forward declarations
namespace Ui { class MainWindow; class SimulatedAnnealingDialog; }
class QStandardItem;
class QStandardItemModel;
class TargetManager;
class ComputationManager;

class MainWindow :
    public QMainWindow
{
Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

signals:
    void parsingRequested(const QString& filename, const QString& parsername);
    void computationRequested(const QList<TargetDataQt>&, double, const QString&);
    void simulatedAnnealingCoolingScheduleChanged(const std::tuple<double, double, unsigned int, double>&);
    void computationStopRequested();

public slots:
    void logToConsole(const QString&);

#if 0
    void parserThreadStarted();
    void parserThreadFinished();

    void addTarget(const TargetDataQt&);
#endif


private slots:
//    void _datafileOpen();
//    void _datafileLoad();

    void _addTargetObject(QStandardItem*);

    void _changeMethodType(const QString&);
    void _showMethodOptionsDialog();
    void _methodOptionsDialogFinished(int);

    void _computationCommand();

    void _updateVelocityFromUnits(double);
    void _updateVelocityFromFractions(double);

    void _updateNumberOfTargets(int);

    void _computationStarted();
    void _computationFinished();

    void _displayPath(const QList<std::array<double,3>>&, double time, double length);

private:
    Ui::MainWindow* _ui;
    TargetManager* _targetManager;
    ComputationManager* _computationManager;

    Ui::SimulatedAnnealingDialog* _uiSADialog;
    QDialog* _saDialog;

    QStandardItemModel* _targetsModel;

    bool _computationRunning;
};

#endif // MAIN_WINDOW_HH
