/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef DATA_LOADER_CSV_HH
#define DATA_LOADER_CSV_HH

#include <QString>

#include "parser/csv.hh"
#include "target_data.hh"

class DataLoaderCSV :
    public QObject
{
    Q_OBJECT
public:
    DataLoaderCSV() :
        QObject()
    {
    }

public slots:
    void load(const QString& filename);

signals:
    void log(const QString&);
    void targetFound(const TargetDataQt&);
    void finished();

private:
    bool csv_func(const CSV::Parser::data& d);
};


#endif // DATA_LOADER_CSV_HH
