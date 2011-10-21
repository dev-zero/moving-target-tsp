/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef EXHAUSTIVE_SEARCH_HH
#define EXHAUSTIVE_SEARCH_HH

#include <QtCore/QList>
#include "base/target_data.hh"
#include <vector>

class ExhaustiveSearch
{
public:
    ExhaustiveSearch(const QList<TargetDataQt>& targets, double velocity, const TargetDataQt& origin, double start_time = 0.0);

    void step(); // calculate and evaluate next possible tour

    bool done() const; // returns true if all possibilities have been evaluated

    void compute_all();

    bool found_solution() const;

    // returns the position of the n'th path target at the time it is hit
    std::array<double,3> get_shortest_path_target_position(size_t n);

    double getSolutionTime() const;

private:
    bool _done;
    const QList<TargetDataQt>& _targets; // TODO: dangerous
    std::vector<size_t> _permutation, _shortest_path_permutation;
    double _shortest_path_time;
    std::vector<double> _shortest_path_times;
    const double _velocity;
    const TargetDataQt _origin;
    const double _start_time;
    bool _found_solution;
};


#endif // EXHAUSTIVE_SEARCH_HH
