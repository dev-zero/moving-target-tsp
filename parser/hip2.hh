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
#include <tuple>

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

    /* spec:
     * same as for the functor above */
    typedef std::tuple<int, int, double, double, double, double, double, double, double> data;
    typedef std::function<bool (const data&)> appender_tuple_func;
    void operator()(const std::string& text, appender_tuple_func func);
};

enum fields
{
    ID = 0,
    SOLUTIONTYPE,
    RIGHT_ASCENSION,
    DECLINATION,
    PARALLAX,
    PARALLAX_ERR,
    PROPER_MOTION_IN_RA,
    PROPER_MOTION_IN_DECL,
    COLOUR_INDEX
};

}
#endif // HIP2_HH
