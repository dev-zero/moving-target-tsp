/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef EXHAUSTIVE_SEARCH_RECURSIVE_HH
#define EXHAUSTIVE_SEARCH_RECURSIVE_HH

#include <vector>
#include <deque>

#include <QtCore/QObject>

#include "base/target_data.hh"

class ExhaustiveSearchRecursive :
    public QObject
{
    Q_OBJECT
public:
    ExhaustiveSearchRecursive(QObject* p = 0);

    /**
     * 
     * @param targets The complete list of targets (including the origin)
     * @param velocity The velocity for the traveller
     * @param origin The indeox of the origin (and final) target in targets
     * @param start_time The time to start the tour at
     */
    void set(std::vector<TargetDataQt> const * targets, double velocity, size_t origin = 0, double start_time = 0.0);

    /**
     * whether a valid solution has been found
     */
    bool solutionFound() const;

    /**
     * returns a list of (time, index) pairs denoting the
     * time 'time' at which the target 'index' in the list of targets
     * provided in set() is encountered.
     * An empty list if no solution was found (but better check with solutionFound())
     */
    std::deque<std::pair<double, size_t>> getSolution() const;

public slots:
    /**
     * abort current calculations
     * only if calculations are running in a separate thread,
     * otherwise the signal won't be delivered, resp. you are unable
     * to call abort()
     */
    void abort();

    /**
     * evaluate/calculate all possible tours
     */
    void evaluate();

signals:
    /**
     * when finished
     */
    void finished();

private:
    std::vector<TargetDataQt> const * _targets;
    double _velocity;
    size_t _origin;
    double _start_time;
    bool _abort;
    bool _finished;

    double _best_time;
    std::deque<std::pair<double, size_t>> _optimal_tour, _current_tour; // contains list of (time, index)

    /**
     * calculates a non-closed subtour (the recursive step)
     * @param start The index of the start target
     * @param time The start time for the targets
     * @param targets The list of indices of the targets to lay the tour through
     */
    void _calculate_subtour(size_t start, double time, const std::vector<size_t>& targets);


};

#endif // EXHAUSTIVE_SEARCH_RECURSIVE_HH
