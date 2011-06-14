/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef COMMON_CALCULATION_FUNCTIONS_HH
#define COMMON_CALCULATION_FUNCTIONS_HH

#include <boost/numeric/ublas/vector.hpp>
#include <array>
#include "target.hh"

inline std::array<double,3> operator*(const double& lhs, const std::array<double,3>& rhs)
{
    std::array<double,3> ret = {{ lhs*rhs[0], lhs*rhs[1], lhs*rhs[2] }};
    return ret;
}

inline std::array<double,3> operator+(const std::array<double,3>& lhs, const std::array<double,3>& rhs)
{
    std::array<double,3> ret = {{ lhs[0]+rhs[0], lhs[1]+rhs[1], lhs[2]+rhs[2] }};
    return ret;
}

inline std::array<double,3> operator-(const std::array<double,3>& lhs, const std::array<double,3>& rhs)
{
    std::array<double,3> ret = {{ lhs[0]-rhs[0], lhs[1]-rhs[1], lhs[2]-rhs[2] }};
    return ret;
}


inline double inner_prod(const std::array<double,3>& lhs, const std::array<double,3>& rhs)
{
    return lhs[0]*rhs[0] + lhs[1]*rhs[1] + lhs[2]*rhs[2];
}

/* calculate the time duration to go from s_position to e_position which moves with e_velocity when travelling with velocity v */
double calculate_time(const double& v,
        const boost::numeric::ublas::vector<double>& s_position,
        const boost::numeric::ublas::vector<double>& e_position,
        const boost::numeric::ublas::vector<double>& e_velocity);

double calculate_time(const double& v,
        const std::array<double,3>& s_position,
        const std::array<double,3>& e_position,
        const std::array<double,3>& e_velocity);

/* calculate the distance between two targets
 * and update the shortest_distance if the distance is shorter than shortest_distance */
void calculate_distance_and_direct_travelling_time(double& shortest_distance, double v, const Target& a, const Target& b);

/* convert from equatorial coordinates delta/alpha (declination/right ascension) to galactic coordinates b/l
 *
 * a good resource for astrometric coordinate transformations is:
 * Duffett-Smith, P. "Galactic Coordinates," "Equatorial to Galactic Coordinate Conversion," and "Galactic to Equatorial Coordinate Conversion." §20, 29, and 30 in
 * Practical Astronomy with Your Calculator, 3rd ed. Cambridge, England: Cambridge University Press, pp. 32, 43, and 44, 1992.
 * as cited on http://scienceworld.wolfram.com/astronomy/GalacticCoordinates.html, which yields an even simpler form of the same equations.
 * Constants in both equations are equivalent up to trigonometric identities.
 *
 * Examples sources given to me by my mentor Dr. J. Stadel implement both calculations as follows:
 * * contrib/hip-coordinate-converter/create_coords2.c implements the equation as given in the mentioned book using contants from the append of a paper (link in the source file)
 * * contrib/hip-coordinate-converter/create_coords2.sh implements the equation as given in the Wolfram Scienceworld article (link above)
 *
 * I have decided to implement the latter equation.
 * TODO:
 * * find out why the corresponding constant for C3 in the C source file differs so much
 * * write the inverse function for testing and write test
 * * write a general transformation function using matrices as described in the book
 *
 * Function takes arguments in radian.
 */

namespace equatorial2galactic_constants
{
    const static double C1 = 62.6;
    const static double C2 = 282.25;
    const static double C3 = 33.0;
}

void equatorial2galactic(const double& delta, const double& alpha, double& b, double& l);

void equatorial2cartesian(const double& delta, const double& alpha, const double& plx, const double& plx_err, double& x, double& y, double& z);
#endif // COMMON_CALCULATION_FUNCTIONS_HH
