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
#include "gui/target_data.hh"

int main(int argc, char* argv[])
{
    // register our custom type for Qt meta object usage before everything else
    qRegisterMetaType<TargetDataQt>("TargetDataQt");

    // initialize some osg subsystem settings (aka "yes, I want AA")
    osg::DisplaySettings::instance()->setNumMultiSamples(4);

    QApplication app(argc, argv);
    MainWindow mainWindow;
    ParserThread parserThread;


    // wire things up
    QObject::connect(&parserThread, SIGNAL(log(const QString&)), &mainWindow, SLOT(logToConsole(const QString&)));
    QObject::connect(&parserThread, SIGNAL(started()), &mainWindow, SLOT(parserThreadStarted()));
    QObject::connect(&parserThread, SIGNAL(finished()), &mainWindow, SLOT(parserThreadFinished()));
    QObject::connect(&parserThread, SIGNAL(targetFound(const TargetDataQt&)), &mainWindow, SLOT(addTarget(const TargetDataQt&)));
    QObject::connect(&mainWindow, SIGNAL(parsingRequested(const QString&, const QString&)), &parserThread, SLOT(start(const QString&, const QString&)));

    mainWindow.show();
    return app.exec();
}
