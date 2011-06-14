/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef DATA_LOADER_HIPPARCOS_HH
#define DATA_LOADER_HIPPARCOS_HH

#include <QString>

#include "parser/hip2.hh"
#include "parser/hip.hh"
#include "target_data.hh"

class DataLoaderHipparcos :
    public QObject
{
    Q_OBJECT
public:
    DataLoaderHipparcos() :
        QObject()
    {
    }

public slots:
    void load(const QString& hipparcos2Filename, const QString& hipparcosFilename = "");

signals:
    void targetFound(const TargetDataQt&);
    void finished();

private:
    bool hip2_func(const HIP2::Parser::data& d);
    bool hip_func(const HIP::Parser::data& d);
};


#endif // DATA_LOADER_HIPPARCOS_HH
