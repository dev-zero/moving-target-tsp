/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include "base/exhaustive_search_recursive.hh"

using namespace std;

int main(int argc, char* argv[])
{
    array<double,3> vel = {{ 0.0, 0.0, 0.0 }};
    array<double,3> origin_p = {{ 0.0, 0.0, 0.0 }}, t1_p = {{ 1.0, 0.0, 0.0 }}, t2_p = {{ 1.0, 1.0, 0.0 }}, t3_p = {{ 0.0, 1.0, 0.0 }};
    array<double,3> t1_v = {{ 1.0, 0.0, 0.0 }};
 
    TargetDataQt origin( origin_p, vel, "origin");
    TargetDataQt t1( t1_p, t1_v, "t1");
    TargetDataQt t2( t2_p, vel, "t2");
    TargetDataQt t3( t3_p, vel, "t3");

    vector<TargetDataQt> targets;
    targets.push_back(origin);
    targets.push_back(t1);
    targets.push_back(t2);
    targets.push_back(t3);

    ExhaustiveSearchRecursive search;
    search.set(&targets, 2.0);

    search.evaluate();

    std::cout << "solutionFound? " << search.solutionFound() << std::endl;
    
    return 0;
}

