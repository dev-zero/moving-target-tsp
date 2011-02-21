/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include <cmath>
#include "common_calculation_functions.hh"

namespace ublas
{
    using namespace boost::numeric::ublas;
}

/* calculate the time duration to go from s_position to e_position which moves with e_velocity when travelling with velocity v */
double calculate_time(const double& v, const ublas::vector<double>& s_position, const ublas::vector<double>& e_position, const ublas::vector<double>& e_velocity)
{
    double a(v*v - ublas::inner_prod(e_velocity, e_velocity));
    double b(-2 * ublas::inner_prod(e_position - s_position, e_velocity));
    double c(-ublas::inner_prod(e_position - s_position, e_position - s_position));

    if (std::abs(a) < std::numeric_limits<double>::epsilon())
        return -c / b;
    // TODO: if b^2 =~ 4ac .. use different formula
    double t1( (-b + sqrt(b*b - 4*a*c)) / (2*a) );
    double t2( (-b - sqrt(b*b - 4*a*c)) / (2*a) );

    if (t1 < 0.0)
    {
        if (t2 < 0.0)
            return std::numeric_limits<double>::infinity();
        return t2;
    }
    else if(t2 < 0.0)
        return t1;
    else
        return std::min(t1, t2);
}

/* calculate the time duration to go from s_position to e_position which moves with e_velocity when travelling with velocity v */
double calculate_time(const double& v, const std::array<double,3>& s_position, const std::array<double,3>& e_position, const std::array<double,3>& e_velocity)
{
    double a(v*v - inner_prod(e_velocity, e_velocity));
    double b(-2 * inner_prod(e_position - s_position, e_velocity));
    double c(-inner_prod(e_position - s_position, e_position - s_position));

    if (std::abs(a) < std::numeric_limits<double>::epsilon())
        return -c / b;
    // TODO: if b^2 =~ 4ac .. use different formula
    double t1( (-b + sqrt(b*b - 4*a*c)) / (2*a) );
    double t2( (-b - sqrt(b*b - 4*a*c)) / (2*a) );

    if (t1 < 0.0)
    {
        if (t2 < 0.0)
            return std::numeric_limits<double>::infinity();
        return t2;
    }
    else if(t2 < 0.0)
        return t1;
    else
        return std::min(t1, t2);
}

void calculate_distance_and_direct_travelling_time(double& shortest_distance, double v, const Target& a, const Target& b)
{
    double distance(ublas::norm_1(a.position - b.position));
    std::cout << "current distance between targets " << a.name << " and " << b.name << " is: " << distance << std::endl;
    std::cout << "    direct travelling time: " << calculate_time(v, a.position, b.position, b.velocity) << std::endl;

    if ( (distance > 0.0) && (distance < shortest_distance) ) // ignore the distance if it is 0 (to make sure we have a positiv radius)
        shortest_distance = distance;
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
     * TODO: understand */
    if (plx < 0.5*plx_err)
        r = 1.5/plx_err;
    else
        r = 1.0/plx;

    x = r*cos(b)*cos(l);
    y = r*cos(b)*sin(l);
    z = r*sin(b);
}
