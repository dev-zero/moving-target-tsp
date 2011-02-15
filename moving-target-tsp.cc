/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <QApplication>

#include "gui/main_window.hh"
#include "gui/parser_thread.hh"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWindow;
    ParserThread parserThread;

    // wire things up
    QObject::connect(&parserThread, SIGNAL(log(const QString&)), &mainWindow, SLOT(logToConsole(const QString&)));
    QObject::connect(&parserThread, SIGNAL(started()), &mainWindow, SLOT(parserThreadStarted()));
    QObject::connect(&parserThread, SIGNAL(finished()), &mainWindow, SLOT(parserThreadFinished()));
    QObject::connect(&parserThread, SIGNAL(targetFound(const QString&)), &mainWindow, SLOT(addTarget(const QString&)));
    QObject::connect(&mainWindow, SIGNAL(parsingRequested(const QString&, const QString&)), &parserThread, SLOT(start(const QString&, const QString&)));

    mainWindow.show();
    return app.exec();
}
