/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include "base/target_data.hh"
#include "base/simulated_annealing.hh"
#include "utils/common_calculation_functions.hh"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include "moving-target-tsp.hxx"
#pragma GCC diagnostic pop

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <time.h>

#include <QtCore/QFile>

void usage()
{
    std::cerr
        << "usage: moving-target-tsp-cli [options]"
        << std::endl
        << "options:"
        << std::endl;
    options::print_usage(std::cerr);
}

void printTableHeader()
{
    std::cout << std::endl;
    std::cout
        << std::setw(12) << "time (y)"
        << std::setw(16) << "name"
        << std::setw(14) << "distance (pc)"
        << std::setw(16) << "proper-time (y)"
        << std::setw(16) << "coordinate-time (y)"
        << std::setw(16) << "proper-velocity"
        << std::endl;
    std::cout << std::endl;
}
int main(int argc, char* argv[])
{
    // initialize random number generator
    srand(time(NULL));

    double v;
    unsigned int samplingsteps;
    double finaltemperature;
    double decreasecoefficient;
    unsigned int sametemperaturesteps;
    std::string filename;
    double acceleration;

    try
    {
        int end;
        options o(argc, argv, end);

        if (o.help())
        {
            usage ();
            return 0;
        }

        v = o.velocity();
        samplingsteps = o.sampling();
        finaltemperature = o.temperature();
        decreasecoefficient = o.coefficient();
        sametemperaturesteps = o.sametemperature();
        acceleration = o.acceleration();

        if (o.file().empty())
        {
            std::cerr << "you have to specify a file containing the targets" << std::endl;
            usage();
            return 1;
        }
        filename = o.file();
    }
    catch (const cli::exception& e)
    {
        std::cerr << e << std::endl;
        usage();
        return 1;
    }

    std::cout << "using coordinate velocity: " << v << std::endl;

    QFile file(filename.c_str());
    if (!file.open(QIODevice::ReadOnly))
    {
        std::cerr << "could not open file" << std::endl;
        return 1;
    }

    QDataStream in(&file);

    quint64 magic;
    in >> magic;
    if (magic != 0x4D54545350)
    {
        std::cerr << "invalid file specified" << std::endl;
        return 1;
    }

    qint32 version;
    in >> version;
    if (version != 1)
    {
        std::cerr << "invalid file version" << std::endl;
        return 1;
    }

    int rowCount;
    in >> rowCount;

    std::cout << "loading " << rowCount << " targets ";
    std::vector<TargetDataQt> targets(rowCount);
    for (int i(0); i < rowCount; ++i)
    {
        QString name;
        std::array<double,3> position, velocity;
        QMap<QString, QVariant> t_data;

        in >> name;
        in >> position[0] >> position[1] >> position[2];
        in >> velocity[0] >> velocity[1] >> velocity[2];
        in >> t_data;

        TargetDataQt t(position, velocity, name);
        t.data = t_data;

        targets.at(i) = t;
        std::cout << ".";
        std::flush(std::cout);
    }
    std::cout << " done." << std::endl;

    const std::array<double,3> origin = {{ 0.0, 0.0, 0.0 }};
    targets.push_back(TargetDataQt(origin, origin, "origin"));

    SimulatedAnnealing sim;
    sim.set(&targets, v, rowCount);
    sim.setCoolingParameters(samplingsteps, finaltemperature, decreasecoefficient, sametemperaturesteps);
    sim.evaluate();

    std::cout << "final solution:" << std::endl;
    printTableHeader();

    std::pair<double, size_t> previous(std::make_pair(0.0, rowCount));
    double totalDistance(0.0);
    double totalProperTime(0.0);
    double totalCoordinateTime(0.0);
    size_t count(0);

    for (auto p : sim.getSolution())
    {
        if (count++ % 100 == 0)
            printTableHeader();
        double distance(0.0);

        const TargetDataQt &t2(targets.at(p.second)), &t1(targets.at(previous.second));
        distance = norm(t2.position + p.first*t2.velocity - (t1.position + previous.first*t1.velocity));

        double distanceInLY(distance * 3.261563777);
        double properTime(acosh(1 + acceleration*distanceInLY/2.0) / acceleration); // properTime for half the distance (accel=decel)
        double coordinateTime(sinh(acceleration*properTime)/acceleration); // coordinateTime for half the distance
        double properVelocity(coordinateTime*acceleration);

        properTime *= 2.0; // 2-times half the distance
        coordinateTime *= 2.0; // 2-times half the distance

        std::cout
            << std::setw(12) << p.first
            << std::setw(16) << "\"" + targets.at(p.second).name.toStdString() + "\""
            << std::setw(14) << distance
            << std::setw(16) << properTime
            << std::setw(16) << coordinateTime
            << std::setw(16) << properVelocity
            << std::endl;

        previous = p;
        totalDistance += distance;
        totalProperTime += properTime;
        totalCoordinateTime += coordinateTime;
    }

    std::cout << std::endl;
    std::cout << std::setfill('-') << std::setw(12) << "" << std::setw(16) << "TOTAL" << std::setw(30) << "" << std::endl;
    std::cout << std::setfill(' ');
    std::cout
        << std::setw(12) << previous.first
        << std::setw(16) << " "
        << std::setw(14) << totalDistance
        << std::setw(16) << totalProperTime
        << std::setw(16) << totalCoordinateTime
        << std::endl;

    return 0;
}
