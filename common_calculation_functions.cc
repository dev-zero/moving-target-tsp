/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

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

void calculate_distance_and_direct_travelling_time(double& shortest_distance, double v, const Target& a, const Target& b)
{
    double distance(ublas::norm_1(a.position - b.position));
    std::cout << "current distance between targets " << a.name << " and " << b.name << " is: " << distance << std::endl;
    std::cout << "    direct travelling time: " << calculate_time(v, a.position, b.position, b.velocity) << std::endl;

    if ( (distance > 0.0) && (distance < shortest_distance) ) // ignore the distance if it is 0 (to make sure we have a positiv radius)
        shortest_distance = distance;
}

