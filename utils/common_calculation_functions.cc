/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include "common_calculation_functions.hh"

#include <cmath>
#include <limits>

#ifdef HAVE_BOOST
namespace ublas
{
    using namespace boost::numeric::ublas;
}
#endif

double _calc_min_non_neg_root(const double& a, const double& b, const double& c)
{
    if (std::abs(a) < std::numeric_limits<double>::epsilon())
    {
        if (-c/b > 0.0)
            return -c / b;
        else
            return std::numeric_limits<double>::infinity();
    }

    // TODO: if b^2 =~ 4ac .. use different formula
    double t1( (-b + sqrt(b*b - 4*a*c)) / (2*a) );
    double t2( (-b - sqrt(b*b - 4*a*c)) / (2*a) );

    if (t1 >= 0.0)
    {
        if (t2 >= 0.0)
            return std::min(t1, t2);
        else
            return t1;
    }
    else
    {
        if (t2 >= 0.0)
            return t2;
    }
    return std::numeric_limits<double>::infinity();
}

#ifdef HAVE_BOOST
/* calculate the time duration to go from s_position to e_position which moves with e_velocity when travelling with velocity v */
double calculate_time(const double& v, const ublas::vector<double>& s_position, const ublas::vector<double>& e_position, const ublas::vector<double>& e_velocity)
{
    double a(v*v - ublas::inner_prod(e_velocity, e_velocity));
    double b(-2 * ublas::inner_prod(e_position - s_position, e_velocity));
    double c(-ublas::inner_prod(e_position - s_position, e_position - s_position));
    return _calc_min_non_neg_root(a, b, c);
}

void calculate_distance_and_direct_travelling_time(double& shortest_distance, double v, const Target& a, const Target& b)
{
    double distance(ublas::norm_1(a.position - b.position));
    std::cout << "current distance between targets " << a.name << " and " << b.name << " is: " << distance << std::endl;
    std::cout << "    direct travelling time: " << calculate_time(v, a.position, b.position, b.velocity) << std::endl;

    if ( (distance > 0.0) && (distance < shortest_distance) ) // ignore the distance if it is 0 (to make sure we have a positiv radius)
        shortest_distance = distance;
}

#endif

/* calculate the time duration to go from s_position to e_position which moves with e_velocity when travelling with velocity v */
double calculate_time(const double& v, const std::array<double,3>& s_position, const std::array<double,3>& e_position, const std::array<double,3>& e_velocity)
{
    double a(v*v - inner_prod(e_velocity, e_velocity));
    double b(-2 * inner_prod(e_position - s_position, e_velocity));
    double c(-inner_prod(e_position - s_position, e_position - s_position));
    return _calc_min_non_neg_root(a, b, c);
}

const static double DEG2RAD(M_PI/180.0);
const static double C1(equatorial2galactic_constants::C1 * DEG2RAD);
const static double C2(equatorial2galactic_constants::C2 * DEG2RAD);
const static double C3(equatorial2galactic_constants::C3 * DEG2RAD);

void equatorial2galactic(const double& delta, const double& alpha, double& b, double& l)
{
    const double x( cos(delta)*cos(alpha - C2) );
    const double y( sin(delta)*sin(C1) + cos(delta)*sin(alpha - C2)*cos(C1) );
    const double z( sin(delta)*cos(C1) - cos(delta)*sin(alpha - C2)*sin(C1) );

    l = atan2(y, x) + C3;
    b = atan2(z, sqrt(x*x + y*y));
}

void equatorial2cartesian(const double& delta, const double& alpha, const double& plx, const double& plx_err, double& x, double& y, double& z)
{
    double b(0.0), l(0.0), r(0.0);

    equatorial2galactic(delta, alpha, b, l);

    /* ok, this is stupidly copied from create_coords2.c
     * TODO: understand
     * in principal this gives us the radius in Parsec, is that correct?
     * */
    if (plx < 0.5*plx_err)
        r = 1.5/plx_err;
    else
        r = 1.0/plx;

    r *= 1000.0; // converting milli-parsec to parsec
    x = r*cos(b)*cos(l);
    y = r*cos(b)*sin(l);
    z = r*sin(b);
}

const double seconds_per_year(31557600.0);
const double parsec_in_m(30.856776e15);

double parsec_per_year2meters_per_second(const double& v)
{
    return (v*parsec_in_m)/seconds_per_year;
}

double parsec_per_year2fractions_of_c(const double& v)
{
    return ((v*parsec_in_m)/seconds_per_year)/physical_constants::c;
}

double fractions_of_c2parsec_per_year(const double& v)
{
    return ((v*physical_constants::c)*seconds_per_year)/parsec_in_m;
}

