/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <QApplication>

#include <tuple>
#include <array>

#include <osg/DisplaySettings>

#include "gui/main_window.hh"
#include "gui/target_data.hh"

MainWindow* mainWindowPtr;

void RedirectLogToConsole(QtMsgType, const char* msg)
{
    std::cerr << msg << std::endl;

    if (!mainWindowPtr)
        return;

    QMetaObject::invokeMethod(mainWindowPtr, "logToConsole", Qt::AutoConnection,
            QGenericReturnArgument(), Q_ARG(QString, msg));
}

int main(int argc, char* argv[])
{
    // register our custom type for Qt meta object usage before everything else
    qRegisterMetaType<TargetDataQt>("TargetDataQt");
    qRegisterMetaType<QList<std::array<double,3>>>("QList<std::array<double,3>>");
    qRegisterMetaType<std::tuple<double, double, unsigned int, double>>("std::tuple<double, unsigned int, double, double>");

    // initialize some osg subsystem settings (aka "yes, I want AA")
    osg::DisplaySettings::instance()->setNumMultiSamples(4);

    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindowPtr = &mainWindow;

    qInstallMsgHandler(RedirectLogToConsole);

    mainWindow.show();
    return app.exec();
}
