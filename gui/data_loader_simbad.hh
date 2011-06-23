/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef DATA_LOADER_SIMBAD_HH
#define DATA_LOADER_SIMBAD_HH

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

#include "gui/target_data.hh"

/// forward declaration
class QtSoapHttpTransport;

class DataLoaderSimbad :
    public QObject
{
    Q_OBJECT
public:
    DataLoaderSimbad();
    ~DataLoaderSimbad();
public slots:
    void query(const QString& query);
    void query(const QStringList& queries);
signals:
    void finished();
    void targetFound(const TargetDataQt&, const QStringList&);
    void queryError(const QString&);

private slots:
    void _getSoapResponse();
private:
    QtSoapHttpTransport* _http;
};

#endif // DATA_LOADER_SIMBAD_HH
