/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <QtXml/QDomDocument>

#include "3rdparty/qtsoap/qtsoap.h"

#include "base/data_loader_simbad.hh"
#include "utils/common_calculation_functions.hh"

DataLoaderSimbad::DataLoaderSimbad() :
    QObject()
{
    _http = new QtSoapHttpTransport(this);
    _http->setAction("urn:simbad");
    _http->setHost("cdsws.u-strasbg.fr");

    connect(_http, SIGNAL(responseReady()), SLOT(_getSoapResponse()));
}

DataLoaderSimbad::~DataLoaderSimbad()
{
    delete _http;
}

void DataLoaderSimbad::query(const QString& q)
{
    QtSoapMessage request;
    request.setMethod(QtSoapQName("sesame", "urn:sesame"));
    request.addMethodArgument("name", "", q);
    request.addMethodArgument("resultType", "", "x");
    _http->submitRequest(request, "/axis/services/Sesame");
}

void DataLoaderSimbad::query(const QStringList& qs)
{
    QString q;
    foreach (q, qs)
    {
        query(q);
    }

}

void DataLoaderSimbad::_getSoapResponse()
{
    const QtSoapMessage& resp(_http->getResponse());
    if (resp.isFault())
    {
        emit queryError("soap request failed: " + resp.faultString().value().toString());
        return;
    }

    const QtSoapType& res(resp.returnValue());
    if (!res.isValid())
    {
        emit queryError("soap return type is invalid");
        return;
    }

    qDebug() << res.value().toString();

    QDomDocument doc;
    doc.setContent(res.value().toString());


    QDomNodeList targets(doc.elementsByTagName("Target"));

    for (int i(0); i < targets.count(); ++i)
    {
        QDomNode target_node(targets.at(i));
        QDomNode resolver = target_node.namedItem("Resolver");

        if (resolver.isNull())
        {
            qDebug() << "no object found";
            continue;
        }

        QStringList data;

        data << resolver.firstChildElement("oname").text();

        data << resolver.firstChildElement("jradeg").text();
        data << resolver.firstChildElement("jdedeg").text();

        QDomNode radialVelocity = resolver.namedItem("Vel");
        if (radialVelocity.isNull())
            data << "(unavailable)";
        else
            data << radialVelocity.firstChildElement("v").text();

        QDomNode properMotions = resolver.namedItem("pm");
        QDomNode parallax = resolver.namedItem("plx");
        if (properMotions.isNull() || parallax.isNull())
        {
            emit queryError("a target was found but does not contain proper motions and/or parallax data");
            return;
        }
        else
        {
            data << parallax.firstChildElement("v").text();
            data << properMotions.firstChildElement("pmRA").text();
            data << properMotions.firstChildElement("pmDE").text();

            // we need the parallax contained in the proper motions to calculate the cartesian coordinates

            std::array<double,3> position, velocity;
            equatorial2cartesian(
                    deg2rad(resolver.firstChildElement("jdedeg").text().toDouble()),
                    deg2rad(resolver.firstChildElement("jradeg").text().toDouble()),
                    parallax.firstChildElement("v").text().toDouble(),
                    parallax.firstChildElement("e").text().toDouble(),
                    position[0], position[1], position[2]);
            equatorial2cartesian(
                    deg2rad(resolver.firstChildElement("jdedeg").text().toDouble()) + mas2rad(properMotions.firstChildElement("pmDE").text().toDouble()),
                    deg2rad(resolver.firstChildElement("jradeg").text().toDouble()) + mas2rad(properMotions.firstChildElement("pmRA").text().toDouble()),
                    parallax.firstChildElement("v").text().toDouble(),
                    parallax.firstChildElement("e").text().toDouble(),
                    velocity[0], velocity[1], velocity[2]);

            velocity[0] -= position[0];
            velocity[1] -= position[1];
            velocity[2] -= position[2];

            data << QString("%1/%2/%3").arg(position[0]).arg(position[1]).arg(position[2]);
            data << QString("%1/%2/%3").arg(velocity[0]).arg(velocity[1]).arg(velocity[2]);

            TargetDataQt t(position, velocity, resolver.firstChildElement("oname").text());
            emit targetFound(t, data);
        }
    }
}


