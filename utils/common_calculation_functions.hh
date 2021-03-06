/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef COMMON_CALCULATION_FUNCTIONS_HH
#define COMMON_CALCULATION_FUNCTIONS_HH

#ifdef HAVE_BOOST
#include <boost/numeric/ublas/vector.hpp>
#include "base/target.hh"
#endif

#include <array>
#include <cmath>

/**
 * element-wise multiplication between a scalar and a 3-vector
 */
inline std::array<double,3> operator*(const double& lhs, const std::array<double,3>& rhs)
{
    std::array<double,3> ret = {{ lhs*rhs[0], lhs*rhs[1], lhs*rhs[2] }};
    return ret;
}

/**
 * element-wise addition of two vectors
 */
inline std::array<double,3> operator+(const std::array<double,3>& lhs, const std::array<double,3>& rhs)
{
    std::array<double,3> ret = {{ lhs[0]+rhs[0], lhs[1]+rhs[1], lhs[2]+rhs[2] }};
    return ret;
}

/**
 * element-wise subtraction of two vectors
 */
inline std::array<double,3> operator-(const std::array<double,3>& lhs, const std::array<double,3>& rhs)
{
    std::array<double,3> ret = {{ lhs[0]-rhs[0], lhs[1]-rhs[1], lhs[2]-rhs[2] }};
    return ret;
}

/**
 * inner-product (scalar product) of two vectors using the standard norm
 */
inline double inner_prod(const std::array<double,3>& lhs, const std::array<double,3>& rhs)
{
    return lhs[0]*rhs[0] + lhs[1]*rhs[1] + lhs[2]*rhs[2];
}

/**
 * the norm using the standard norm
 */
inline double norm(const std::array<double,3>& v)
{
    return sqrt(inner_prod(v, v));
}

#ifdef HAVE_BOOST

/**
 * calculate the time duration to go from s_position to e_position which moves with e_velocity when travelling with velocity v
 */
double calculate_time(const double& v,
        const boost::numeric::ublas::vector<double>& s_position,
        const boost::numeric::ublas::vector<double>& e_position,
        const boost::numeric::ublas::vector<double>& e_velocity);

/**
 * calculate the distance between two targets and update the shortest_distance if the distance is shorter than shortest_distance
 */
void calculate_distance_and_direct_travelling_time(double& shortest_distance, double v, const Target& a, const Target& b);

#endif

/**
 * calculate the time needed to go from a given position to a different moving position
 * \param v the velocity of the traveller
 * \param s_position the start position of the traveller
 * \param e_position the target's position at the start time
 * \param e_velocity the target's velocity
 */
double calculate_time(const double& v,
        const std::array<double,3>& s_position,
        const std::array<double,3>& e_position,
        const std::array<double,3>& e_velocity);

/**
 * \page coordinate-transformation Converting from equatorial coordinates delta/alpha (declination/right ascension) to galactic coordinates b/l
 *
 * A good resource for astrometric coordinate transformations is:
 * Duffett-Smith, P. "Galactic Coordinates," "Equatorial to Galactic Coordinate Conversion," and "Galactic to Equatorial Coordinate Conversion." §20, 29, and 30 in
 * Practical Astronomy with Your Calculator, 3rd ed. Cambridge, England: Cambridge University Press, pp. 32, 43, and 44, 1992.
 * as cited on http://scienceworld.wolfram.com/astronomy/GalacticCoordinates.html, which yields an even simpler form of the same equations.
 * Constants in both equations are equivalent up to trigonometric identities.
 *
 * Examples sources given to me by my mentor Dr. J. Stadel implement both calculations as follows:
 * - contrib/hip-coordinate-converter/create_coords2.c implements the equation as given in the mentioned book using contants from the append of a paper (link in the source file)
 * - contrib/hip-coordinate-converter/create_coords2.sh implements the equation as given in the Wolfram Scienceworld article (link above)
 *
 * I have decided to implement the latter equation.
 *
 * TODO:
 * - find out why the corresponding constant for C3 in the C source file differs so much
 * - write the inverse function for testing and write test
 * - write a general transformation function using matrices as described in the book
 *
 */

namespace equatorial2galactic_constants
{
    const static double C1 = 62.6;
    const static double C2 = 282.25;
    const static double C3 = 33.0;
}

namespace physical_constants
{
    /// speed of light [m/s]
    const static double c = 299792458.0;
}

namespace unit_conversions
{
    const static double lightyearsPerParsec = 3.261563777;
}

/**
 * Converting from Equatorial to Galactic coordinates
 * \param delta declination [rad]
 * \param alpha right ascension [rad]
 */
void equatorial2galactic(const double& delta, const double& alpha, double& b, double& l);

/**
 * Converting from Equatorial to Cartesian coordinates
 * \param delta declination [rad]
 * \param alpha right ascension [rad]
 * \param plx parallax [mas]
 * \param plx_err error on parallax [mas]
 * \param x x coordinate [pc]
 * \param y y coordinate [pc]
 * \param z z coordinate [pc]
 */
void equatorial2cartesian(const double& delta, const double& alpha, const double& plx, const double& plx_err, double& x, double& y, double& z);

double parsec_per_year2fractions_of_c(const double& v);
double fractions_of_c2parsec_per_year(const double& v);
double parsec_per_year2meters_per_second(const double& v);

inline double deg2rad(const double& d)
{
    return (d/180.0)*M_PI;
}
inline double mas2rad(const double& d)
{
    return (d/648000000.0)*M_PI;
}

inline double parsec2lightyears(const double& d)
{
    return d*unit_conversions::lightyearsPerParsec;
}

inline double lightyears2parsec(const double& d)
{
    return d/unit_conversions::lightyearsPerParsec;
}
#endif // COMMON_CALCULATION_FUNCTIONS_HH
