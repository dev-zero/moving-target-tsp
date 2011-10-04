/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef TARGET_DATA_HH
#define TARGET_DATA_HH

#include <array>
#include <unordered_map>
#include <string>
#include <boost/variant.hpp>

struct TargetData
{
    const std::array<double,3> position;
    const std::array<double,3> velocity;
    const std::string name;

    // can contain additional data
    std::unordered_map<std::string, boost::variant<int, double, std::string>> data;

    TargetData(const std::array<double,3>& p, const std::array<double,3>& v, const std::string& n) :
        position(p),
        velocity(v),
        name(n)
    {
    }
};

/* and a corresponding type using Qt types */

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QMap>

struct TargetDataQt
{
public:
    const std::array<double,3> position;
    const std::array<double,3> velocity;
    const QString name;

    // can contain additional data
    QMap<QString, QVariant> data;

    /* required to register the type in the Qt Meta Data System */
    TargetDataQt() :
        position(),
        velocity(),
        name("")
    {
    }

    TargetDataQt(const std::array<double,3>& p, const std::array<double,3>& v, const QString& n) :
        position(p),
        velocity(v),
        name(n)
    {
    }

    TargetDataQt(const TargetDataQt& rhs) :
        position(rhs.position),
        velocity(rhs.velocity),
        name(rhs.name),
        data(rhs.data)
    {
    }

    TargetDataQt& operator=(const TargetDataQt& rhs)
    {
        // TODO: thought that C++0x has a specifier to allow member functions write public const member
        const_cast<std::array<double,3>&>(position) = rhs.position;
        const_cast<std::array<double,3>&>(velocity) = rhs.velocity;
        const_cast<QString&>(name) = rhs.name;
        data = rhs.data;
        return *this;
    }
};

Q_DECLARE_METATYPE(TargetDataQt)

#endif // TARGET_DATA_HH
