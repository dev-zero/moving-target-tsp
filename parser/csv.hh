/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2010 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef CSV_HH
#define CSV_HH

#include <functional>
#include <string>
#include <exception>

namespace CSV {

struct InvalidLine :
    public std::exception
{
};

struct Parser
{
    typedef std::function<bool (const double&, const double&, const double&, const double&, const double&, const double&, const std::string&)> appender_func;
    void operator()(const std::string& text, appender_func func);

    typedef std::tuple<double, double, double, double, double, double, std::string> data;
    typedef std::function<bool (const data&)> appender_tuple_func;
    void operator()(const std::string& text, appender_tuple_func);
};

}
#endif // CSV_HH
