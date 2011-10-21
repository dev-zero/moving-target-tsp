/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <array>

#include <QtCore/QCoreApplication>

#include "base/computation_manager.hh"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    ComputationManager comp;
    app.connect(&comp, SIGNAL(finished()), SLOT(quit()));

    QList<TargetDataQt> targets;

    std::array<double,3> vel = {{ 0.0, 0.0, 0.0 }};
    std::array<double,3> t1_p = {{ 1.0, 0.0, 0.0 }}, t2_p = {{ 1.0, 1.0, 0.0 }}, t3_p = {{ 0.0, 1.0, 0.0 }};
    std::array<double,3> t1_v = {{ 1.0, 0.0, 0.0 }};
 
    targets << TargetDataQt( t1_p, t1_v, "t1");
    targets << TargetDataQt( t2_p, vel, "t2");
    targets << TargetDataQt( t3_p, vel, "t3");

    comp.setData(targets, 2.0);
    comp.start();

    return app.exec();
}
