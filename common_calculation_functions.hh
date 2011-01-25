/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef COMMON_CALCULATION_FUNCTIONS_HH
#define COMMON_CALCULATION_FUNCTIONS_HH

#include <boost/numeric/ublas/vector.hpp>
#include "target.hh"

/* calculate the time duration to go from s_position to e_position which moves with e_velocity when travelling with velocity v */
double calculate_time(const double& v,
        const boost::numeric::ublas::vector<double>& s_position,
        const boost::numeric::ublas::vector<double>& e_position,
        const boost::numeric::ublas::vector<double>& e_velocity);

/* calculate the distance between two targets
 * and update the shortest_distance if the distance is shorter than shortest_distance */
void calculate_distance_and_direct_travelling_time(double* shortest_distance, double v, const Target& a, const Target& b);

#endif // COMMON_CALCULATION_FUNCTIONS_HH
