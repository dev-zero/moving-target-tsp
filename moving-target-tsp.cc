/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <QApplication>

#include <osg/DisplaySettings>

#include "gui/main_window.hh"
#include "gui/parser_thread.hh"
#include "gui/computation_thread.hh"
#include "gui/target_data.hh"

int main(int argc, char* argv[])
{
    // register our custom type for Qt meta object usage before everything else
    qRegisterMetaType<TargetDataQt>("TargetDataQt");
    qRegisterMetaType<QList<std::array<double,3>>>("QList<std::array<double,3>>");

    // initialize some osg subsystem settings (aka "yes, I want AA")
    osg::DisplaySettings::instance()->setNumMultiSamples(4);

    QApplication app(argc, argv);
    MainWindow mainWindow;
    ParserThread parserThread;
    ComputationThread computationThread;

    // wire things up
    QObject::connect(&parserThread, SIGNAL(log(const QString&)), &mainWindow, SLOT(logToConsole(const QString&)));
    QObject::connect(&parserThread, SIGNAL(started()), &mainWindow, SLOT(parserThreadStarted()));
    QObject::connect(&parserThread, SIGNAL(finished()), &mainWindow, SLOT(parserThreadFinished()));
    QObject::connect(&parserThread, SIGNAL(targetFound(const TargetDataQt&)), &mainWindow, SLOT(addTarget(const TargetDataQt&)));
    QObject::connect(&mainWindow, SIGNAL(parsingRequested(const QString&, const QString&)), &parserThread, SLOT(start(const QString&, const QString&)));
    QObject::connect(&mainWindow, SIGNAL(computationRequested(const QList<TargetDataQt>&, double)), &computationThread, SLOT(start(const QList<TargetDataQt>&, double)));
    QObject::connect(&mainWindow, SIGNAL(computationStopRequested()), &computationThread, SLOT(stop()));
    QObject::connect(&computationThread, SIGNAL(started()), &mainWindow, SLOT(computationThreadStarted()));
    QObject::connect(&computationThread, SIGNAL(finished()), &mainWindow, SLOT(computationThreadFinished()));
    QObject::connect(&computationThread, SIGNAL(log(const QString&)), &mainWindow, SLOT(logToConsole(const QString&)));
    QObject::connect(&computationThread, SIGNAL(solutionFound(const QList<std::array<double,3>>&)), &mainWindow, SLOT(displayPath(const QList<std::array<double,3>>&)));

    mainWindow.show();
    return app.exec();
}
