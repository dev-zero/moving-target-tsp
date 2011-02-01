/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef HIP2_HH
#define HIP2_HH

#include <functional>
#include <string>

namespace HIP2
{
struct InvalidLine :
    public std::exception
{
};


/* please note: this is not a full HIP2 data parser,
 * only what I needed for this work */

struct Parser
{
    /* data is:
     * HIP identifier, solution type, right ascension, declination, parallax, parallax error, proper motion in right ascension, proper motion in declination, colour index */
    typedef std::function<bool (int, int, double, double, double, double, double, double, double)> appender_func;
    void operator()(const std::string& text, appender_func func);
};

}

#endif // HIP2_HH
