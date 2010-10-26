/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef TARGET_HH
#define TARGET_HH

#include <string>
#include <boost/numeric/ublas/vector.hpp>

struct Target
{
    boost::numeric::ublas::vector<double> position, velocity;
    std::string name;

    Target(const double& x, const double& y, const double& z,
            const double& v_x, const double& v_y, const double& v_z,
            const std::string& n) :
        position(3),
        velocity(3),
        name(n)
    {
        position[0] = x;
        position[1] = y;
        position[2] = z;
        velocity[0] = v_x;
        velocity[1] = v_y;
        velocity[2] = v_z;
    }
};

#endif // TARGET_HH
