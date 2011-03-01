/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef MAIN_WINDOW_HH
#define MAIN_WINDOW_HH

#include <QMainWindow>
#include <QItemSelection>
#include <array>
#include <tuple>
#include "target_data.hh"

// START: forward declarations
namespace Ui { class MainWindow; class SimulatedAnnealingDialog; }
class ParserThread;
class QStandardItemModel;
// END: forward declarations

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

    void parserThreadStarted();
    void parserThreadFinished();

    void addTarget(const TargetDataQt&);

    void computationThreadStarted();
    void computationThreadFinished();

    void displayPath(const QList<std::array<double,3>>&);

private slots:
    void _datafileOpen();
    void _datafileLoad();

    void _targetSelectionChanged(const QItemSelection&, const QItemSelection&);

    void _changeMethodType(const QString&);
    void _showMethodOptionsDialog();
    void _methodOptionsDialogFinished(int);

    void _computationCommand();

private:
    Ui::MainWindow* _ui;
    Ui::SimulatedAnnealingDialog* _uiSADialog;
    QDialog* _saDialog;

    QStandardItemModel* _targetsModel;
    qulonglong _targetsTotal;
    qulonglong _targetsSelected;
    bool _computationRunning;

    void _updateTargetNumbers();
};

#endif // MAIN_WINDOW_HH
