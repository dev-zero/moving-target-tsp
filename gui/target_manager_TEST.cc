/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <QtGui/QApplication>
#include <QtGui/QStandardItemModel>

#include "gui/target_manager.hh"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    qRegisterMetaType<TargetDataQt>("TargetDataQt");

    QStandardItemModel targets;
    QStringList header;
    header << "Identifier" << "Position" << "Velocity";
    targets.setHorizontalHeaderLabels(header);
    TargetManager targetManager(&targets);
    targetManager.show();

    app.exec();
}
