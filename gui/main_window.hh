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

// START: forward declarations
namespace Ui { class MainWindow; }
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

public slots:
    void logToConsole(const QString&);

    void parserThreadStarted();
    void parserThreadFinished();

    void addTarget(const QString&);

private slots:
    void _datafileOpen();
    void _datafileLoad();

    void _targetSelectionChanged(const QItemSelection&, const QItemSelection&);
private:
    Ui::MainWindow* _ui;
    QStandardItemModel* _targetsModel;
    qulonglong _targetsTotal;
    qulonglong _targetsSelected;

    void _updateTargetNumbers();
};

#endif // MAIN_WINDOW_HH
