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
     * HIP identifier,
     * solution type new reduction,
     * solution type old reduction,
     * number of components
     * right ascension,
     * declination,
     * parallax,
     * proper motion in right ascension,
     * proper motion in declination,
     * formal error on RArad
     * formal error on DErad
     * formal error on parallax,
     * colour index,
     * formal error on colour index,
     * V-I colour index,
     */
    typedef std::tuple<int, int, short, short, double, double, double, double, double, double, double, double, double, double, double> data;
    typedef std::function<bool (const data&)> appender_tuple_func;
    void operator()(const std::string& text, appender_tuple_func func);
};

enum fields
{
    ID = 0,
    SOLUTIONTYPE,
    SOLUTIONTYPE_OLD_REDUX,
    NUMBER_OF_COMPONENTS,
    RIGHT_ASCENSION,
    DECLINATION,
    PARALLAX,
    PROPER_MOTION_IN_RA,
    PROPER_MOTION_IN_DECL,
    RA_ERROR,
    DECLINATION_ERROR,
    PARALLAX_ERR,
    COLOUR_INDEX,
    COLOUR_INDEX_ERROR,
    VI_COLOUR_INDEX
};

const char field_descriptions[][40] = {
    "Hipparcos identifier",
    "Solution type new reduction",
    "Solution type old reduction",
    "Number of components",
    "Right Ascension in ICRS, Ep=1991.25",
    "Declination in ICRS, Ep=1991.25",
    "Parallax",
    "Proper motion in Right Ascension",
    "Proper motion in Declination",
    "Formal error on RArad",
    "Formal error on DErad",
    "Formal error on Plx",
    "Colour index",
    "Formal error on colour index",
    "V-I colour index"
};

}
#endif // HIP2_HH
