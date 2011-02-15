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
                file.load(std::bind(&ParserThread::csv_null_func, this,
                            std::placeholders::_1,  std::placeholders::_2,  std::placeholders::_3,
                            std::placeholders::_4,  std::placeholders::_5,  std::placeholders::_6,
                            std::placeholders::_7));
            }
            else if (_parsername == "HIP2")
            {
                FileSource<HIP2::Parser> file(_filename.toAscii().data());
                file.load(std::bind(&ParserThread::hip2_null_func, this,
                            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                            std::placeholders::_4, std::placeholders::_5, std::placeholders::_6,
                            std::placeholders::_7, std::placeholders::_8, std::placeholders::_9));
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
    void targetFound(const QString&);
private:
    QString _filename;
    QString _parsername;

    bool hip2_null_func(int id, int, double, double, double, double, double, double, double)
    {
        emit targetFound(QString("hip id %1").arg(id));
        return true;
    }

    bool csv_null_func(const double&, const double&, const double&,
            const double&, const double&, const double&,
            const std::string& name)
    {
        emit targetFound(QString(name.c_str()));
        return true;
    }

};


#endif // PARSER_THREAD_HH
