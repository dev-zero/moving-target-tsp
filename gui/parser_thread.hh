/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef PARSER_THREAD_HH
#define PARSER_THREAD_HH

#include <QThread>
#include <QString>

#include "sources/file_source.hh"
#include "parser/csv.hh"
#include "parser/hip2.hh"
#include "target_data.hh"
#include "common_calculation_functions.hh"

class ParserThread :
    public QThread
{
Q_OBJECT
public:
    ParserThread() :
        QThread()
    {
    }

    void run()
    {
        try
        {
            if (_parsername == "CSV")
            {
                FileSource<CSV::Parser> file(_filename.toAscii().data());
                file.load2tuple(std::bind(&ParserThread::csv_func, this, std::placeholders::_1));
            }
            else if (_parsername == "HIP2")
            {
                FileSource<HIP2::Parser> file(_filename.toAscii().data());
                file.load2tuple(std::bind(&ParserThread::hip2_func, this, std::placeholders::_1));
            }
            else
            {
                emit log(QString("invalid parser specified: %1").arg(_parsername));
            }
        }
        catch (const std::exception& e)
        {
            emit log(QString("an exception occurred while parsing: %1").arg(e.what()));
        }
    }

public slots:
    void start(const QString& filename, const QString& parsername)
    {
        _filename = filename;
        _parsername = parsername;
        QThread::start();
    }
signals:
    void log(const QString&);
    void targetFound(const TargetDataQt&);
private:
    QString _filename;
    QString _parsername;

    bool hip2_func(const HIP2::Parser::data& d)
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

        TargetDataQt t(position, velocity, QString("hip id %1").arg(std::get<HIP2::ID>(d)));

        // hip2 does not have a description, but we might want to know the solutiontype until we get advanced filtering in the GUI
        t.data.insert(QString("description"), QString("solutiontype: %1").arg(std::get<HIP2::SOLUTIONTYPE>(d)));

        emit targetFound(t);
        return true;
    }

    bool csv_func(const CSV::Parser::data& d)
    {
        std::array<double,3> position, velocity;
        std::string name;
        // unpack the tuple to our arrays and name
        std::tie(position[0], position[1], position[2], velocity[0], velocity[1], velocity[2], name) = d;
        emit targetFound(TargetDataQt(position, velocity, QString(name.c_str())));
        return true;
    }

};


#endif // PARSER_THREAD_HH
