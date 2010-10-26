/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef CSV_HH
#define CSV_HH

#include <functional>
#include <string>
#include <exception>

struct InvalidLine :
    public std::exception
{
};

struct CSVParser
{
    typedef std::function<void (const double&, const double&, const double&, const double&, const double&, const double&, const std::string&)> appender_func;
    void operator()(const std::string& text, appender_func func);
};

#endif // CSV_HH
