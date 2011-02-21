/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef COMPUTATION_THREAD_HH
#define COMPUTATION_THREAD_HH

#include <QThread>
#include <QList>

#include <vector>
#include <limits>
#include <algorithm>
#include <array>

#include "common_calculation_functions.hh"
#include "gui/target_data.hh"

class ExhaustiveSearch
{
public:
    ExhaustiveSearch(const QList<TargetDataQt>& targets, double velocity, const TargetDataQt& origin, double start_time = 0.0) :
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

    void step() // calculate and evaluate next possible tour
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

    bool done() const // returns true if all possibilities have been evaluated
    {
        return _done;
    }

    void compute_all()
    {
        while (!done())
            step();
    }

    bool found_solution() const
    {
        return _found_solution;
    }

    // returns the position of the n'th path target at the time it is hit
    std::array<double,3> get_shortest_path_target_position(size_t n)
    {
        size_t m(_shortest_path_permutation.at(n));
        return _targets[m].position + _shortest_path_times[n+1]*_targets[m].velocity;
    }

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

class ComputationThread :
    public QThread
{
Q_OBJECT
public:
    void run()
    {
        _stop = false;

        std::array<double,3> p_and_v = {{ 0.0, 0.0, 0.0 }};

        ExhaustiveSearch es(_targets, _velocity, TargetDataQt(p_and_v, p_and_v, "origin"));

        while (!_stop && !es.done())
            es.step();

        if (es.done() && es.found_solution())
        {
            QList<std::array<double,3>> list;
            const std::array<double,3> origin = {{ 0.0, 0.0, 0.0 }};
            list << origin;
            for (long n(0); n < _targets.size(); ++n)
            {
                list << es.get_shortest_path_target_position(n);
            }
            list << origin;
            emit solutionFound(list);
        }
    }

public slots:
    void start(const QList<TargetDataQt>& targets, double velocity)
    {
        _targets = targets;
        _velocity = velocity;
        emit log(QString("calculating tour for %1 targets").arg(_targets.size()));
        QThread::start();
    }
    void stop()
    {
        _stop = true;
    }

signals:
    void log(const QString&);
    void solutionFound(const QList<std::array<double,3>>&);

private:
    bool _stop;
    QList<TargetDataQt> _targets;
    double _velocity;
};
#endif // COMPUTATION_THREAD_HH
