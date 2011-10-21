/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <QDebug>

#include "base/data_loader_hipparcos.hh"
#include "sources/file_source.hh"
#include "utils/common_calculation_functions.hh"
#include "parser/hip.hh"

template<typename T, size_t size>
struct append_additional_data_helper
{
    static void append(QMap<QString, QVariant>& map, const T& t)
    {
        map.insert(HIP2::field_descriptions[size-1], std::get<size-1>(t));
        append_additional_data_helper<T, size-1>::append(map, t);
    }
};

template<typename T>
struct append_additional_data_helper<T, 0>
{
    static void append(QMap<QString, QVariant>&, const T&)
    {
    }
};

template<typename T>
void append_additional_data(QMap<QString, QVariant>& map, const T& t)
{
    append_additional_data_helper<T, std::tuple_size<T>::value>::append(map, t);
}

void DataLoaderHipparcos::load(const QString& hipparcos2Filename, const QString& hipparcosFilename)
{
    qDebug() << "DataLoaderHipparcos::load";
    try
    {
        if (hipparcosFilename.length() > 0)
        {
            FileSource<HIP::Parser> hipparcosFile(hipparcosFilename.toAscii().data());
            hipparcosFile.load2tuple(std::bind(&DataLoaderHipparcos::hip_func, this, std::placeholders::_1));
        }
        FileSource<HIP2::Parser> hipparcos2File(hipparcos2Filename.toAscii().data());
        hipparcos2File.load2tuple(std::bind(&DataLoaderHipparcos::hip2_func, this, std::placeholders::_1));
    }
    catch (const std::exception& e)
    {
        qDebug() << QString("an exception occurred while parsing: %1").arg(e.what());
    }
    emit finished();
}

bool DataLoaderHipparcos::hip2_func(const HIP2::Parser::data& d)
{
    std::array<double,3> position, velocity;

    equatorial2cartesian(std::get<HIP2::DECLINATION>(d), std::get<HIP2::RIGHT_ASCENSION>(d),
            std::get<HIP2::PARALLAX>(d), std::get<HIP2::PARALLAX_ERR>(d), position[0], position[1], position[2]);
    equatorial2cartesian(std::get<HIP2::DECLINATION>(d) + (std::get<HIP2::PROPER_MOTION_IN_DECL>(d)/(3600000.0*180.0)*M_PI),
            std::get<HIP2::RIGHT_ASCENSION>(d) + (std::get<HIP2::PROPER_MOTION_IN_RA>(d)/(3600000.0*180.0)*M_PI),
            std::get<HIP2::PARALLAX>(d), std::get<HIP2::PARALLAX_ERR>(d), velocity[0], velocity[1], velocity[2]);
    velocity[0] -= position[0];
    velocity[1] -= position[1];
    velocity[2] -= position[2];

    TargetDataQt t(position, velocity, QString("%1").arg(std::get<HIP2::ID>(d)));

    // hip2 does not have a description, but we might want to know the solutiontype until we get advanced filtering in the GUI
    //t.data.insert(QString("description"), QString("solutiontype: %1").arg(std::get<HIP2::SOLUTIONTYPE>(d)));
    append_additional_data(t.data, d);

    emit targetFound(t);
    return true;
}

bool DataLoaderHipparcos::hip_func(const HIP::Parser::data& d)
{
    qDebug() << "Found hipparcos entry: " << std::get<HIP::HIP>(d);
    return true;
}

