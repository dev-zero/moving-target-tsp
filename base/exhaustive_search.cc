/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include "base/exhaustive_search.hh"
#include "utils/common_calculation_functions.hh"

#include <limits>
#include <algorithm>

ExhaustiveSearch::ExhaustiveSearch(const QList<TargetDataQt>& targets, double velocity, const TargetDataQt& origin, double start_time) :
    _done(false),
    _targets(targets),
    _permutation(targets.size()),
    _shortest_path_permutation(targets.size()),
    _shortest_path_time(std::numeric_limits<double>::infinity()),
    _shortest_path_times(targets.size()+2),
    _velocity(velocity),
    _origin(origin),
    _start_time(start_time),
    _found_solution(false)
{
    size_t count(0);
    std::generate(_permutation.begin(), _permutation.end(), [&count]() { return count++; });
}

void ExhaustiveSearch::step() // calculate and evaluate next possible tour
{
    double time(_start_time);
    size_t current(0), next(*_permutation.begin());
    std::vector<double> times(_shortest_path_times.size());
    size_t times_count(0);
    times[times_count++] = time;

    // calculate time from origin to the first target
    time += calculate_time(
            _velocity,
            _origin.position + time*_origin.velocity,
            _targets[next].position + time*_targets[next].velocity,
            _targets[next].velocity);

    times[times_count++] = time;

    for (auto j(_permutation.begin()), j_end(_permutation.end()-1); j != j_end; ++j)
    {
        current = *j;
        next = *(j+1);
        time += calculate_time(_velocity,
                _targets[current].position + time*_targets[current].velocity,
                _targets[next].position + time*_targets[next].velocity,
                _targets[next].velocity);
        times[times_count++] = time;
    }
    // 'next' contains the last defined element at this point

    // calculate the time from the last target back to the origin
    time += calculate_time(
            _velocity,
            _targets[next].position + time*_targets[next].velocity,
            _origin.position + time*_origin.velocity,
            _origin.velocity);
    times[times_count++] = time;

    if (time < _shortest_path_time)
    {
        _shortest_path_time = time;
        _shortest_path_permutation.assign(_permutation.begin(), _permutation.end());
        std::swap(_shortest_path_times, times);
        _found_solution = true;
    }

    _done = !std::next_permutation(_permutation.begin(), _permutation.end());
}

bool ExhaustiveSearch::done() const // returns true if all possibilities have been evaluated
{
    return _done;
}

void ExhaustiveSearch::compute_all()
{
    while (!done())
        step();
}

bool ExhaustiveSearch::found_solution() const
{
    return _found_solution;
}

// returns the position of the n'th path target at the time it is hit
std::array<double,3> ExhaustiveSearch::get_shortest_path_target_position(size_t n)
{
    size_t m(_shortest_path_permutation.at(n));
    return _targets[m].position + _shortest_path_times[n+1]*_targets[m].velocity;
}

double ExhaustiveSearch::getSolutionTime() const
{
    return _shortest_path_time;
}

