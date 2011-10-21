/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <QDebug>

#include <string>
#include <array>

#include "base/data_loader_csv.hh"
#include "sources/file_source.hh"

void DataLoaderCSV::load(const QString& filename)
{
    qDebug() << "DataLoaderCSV::load";
    try
    {
        FileSource<CSV::Parser> file(filename.toAscii().data());
        file.load2tuple(std::bind(&DataLoaderCSV::csv_func, this, std::placeholders::_1));
    }
    catch (const std::exception& e)
    {
        emit log(QString("an exception occurred while parsing: %1").arg(e.what()));
    }
    emit finished();
}

bool DataLoaderCSV::csv_func(const CSV::Parser::data& d)
{
    qDebug() << "DataLoaderCSV::csv_func";
    std::array<double,3> position, velocity;
    std::string name;
    // unpack the tuple to our arrays and name
    std::tie(position[0], position[1], position[2], velocity[0], velocity[1], velocity[2], name) = d;
    emit targetFound(TargetDataQt(position, velocity, QString(name.c_str())));
    return true;
}


