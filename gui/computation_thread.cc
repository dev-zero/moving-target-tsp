/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */



#include <vector>
#include <limits>
#include <algorithm>

// EO
#include <eo>
#include <ga.h>
#include <eoInt.h>
#include <eoInit.h>
#include <eoScalarFitness.h>

// ParadisEO-MO
#include <eval/moFullEvalByCopy.h>
//#include <coolingSchedule/moCoolingSchedule.h>
#include <algo/moSA.h>
#include <comparator/moSolNeighborComparator.h>
#include <problems/permutation/moShiftNeighbor.h>
#include <neighborhood/moRndWithReplNeighborhood.h>

#include "common_calculation_functions.hh"
#include "computation_thread.hh"

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

/* represent our Moving Target TSP by a permutation,
 * the time duration of our tour will be the fitness
 * and the target is to minimize the time */
typedef eoInt<eoMinimizingFitness> MovingTargetTSP;

typedef moShiftNeighbor<MovingTargetTSP> ShiftNeighbor; //shift Neighbor
typedef moRndWithReplNeighborhood<ShiftNeighbor> RndShiftNeighborhood; //rnd shift Neighborhood (Indexed)


template<class EOT>
class MovingTargetTSPEval :
    public eoEvalFunc<EOT>
{
private:
    const QList<TargetDataQt>& _targets;
    const double _velocity;
    const TargetDataQt& _origin;
    const double _start_time;
public:
    MovingTargetTSPEval(const QList<TargetDataQt>& targets, double velocity, const TargetDataQt& origin, double start_time = 0.0) :
        _targets(targets),
        _velocity(velocity),
        _origin(origin),
        _start_time(start_time)
    {
    }

    // fitness function
    void operator()(EOT& _movingtargettsp)
    {
        double time(_start_time);
        size_t current(0), next(_movingtargettsp[0]);

        // calculate time from origin to the first target
        time += calculate_time(
                _velocity,
                _origin.position + time*_origin.velocity,
                _targets[next].position + time*_targets[next].velocity,
                _targets[next].velocity);

        for (unsigned int i(0), i_end(_movingtargettsp.size()-1); i < i_end; ++i)
        {
            current = _movingtargettsp[i];
            next = _movingtargettsp[i+1];
            time += calculate_time(_velocity,
                    _targets[current].position + time*_targets[current].velocity,
                    _targets[next].position + time*_targets[next].velocity,
                    _targets[next].velocity);
        }
        // 'next' contains the last defined element at this point

        // calculate the time from the last target back to the origin
        time += calculate_time(
                _velocity,
                _targets[next].position + time*_targets[next].velocity,
                _origin.position + time*_origin.velocity,
                _origin.velocity);

        _movingtargettsp.fitness(time);
    }
};

class SimulatedAnnealing
{
public:
    SimulatedAnnealing(const QList<TargetDataQt>& targets, double velocity, const TargetDataQt& origin, const std::tuple<double, unsigned int, double, double>& SACoolingSchedule, double start_time = 0.0) :
        _done(false),
        _targets(targets),
        _velocity(velocity),
        _origin(origin),
        _start_time(start_time),
        _found_solution(false),
        _saCoolingSchedule(SACoolingSchedule)
    {
    }

    bool done() const // returns true if all possibilities have been evaluated
    {
        return _done;
    }

    void compute_all()
    {
        MovingTargetTSPEval<MovingTargetTSP> fullEval(_targets, _velocity, _origin, _start_time);
        eoInitPermutation<MovingTargetTSP> init(_targets.size());
        moFullEvalByCopy<ShiftNeighbor> shiftEval(fullEval);
        RndShiftNeighborhood rndShiftNH((_targets.size()-1) * (_targets.size()-1));
        moSimpleCoolingSchedule<MovingTargetTSP> coolingSchedule(
                std::get<0>(_saCoolingSchedule), 
                std::get<1>(_saCoolingSchedule), 
                std::get<2>(_saCoolingSchedule), 
                std::get<3>(_saCoolingSchedule));
        moSA<ShiftNeighbor> localSearch(rndShiftNH, fullEval, shiftEval, coolingSchedule);

        init(_solution);
        fullEval(_solution);

//        std::cout << "initial solution:" << std::endl << _solution << std::endl;
        localSearch(_solution);
//        std::cout << "final solution:" << std::endl << _solution << std::endl;

        _found_solution = true;
        _done = true;
    }

    bool found_solution() const
    {
        return _found_solution;
    }

    // returns the position of the targets at the time they're hit
    QList<std::array<double,3>> get_shortest_path_target_position() const
    {
        QList<std::array<double,3>> list;

        /* TODO: factor this out */
        double time(_start_time);
        size_t current(0), next(_solution[0]);

        list << _origin.position + time*_origin.velocity;

        // calculate time from origin to the first target
        time += calculate_time(
                _velocity,
                _origin.position + time*_origin.velocity,
                _targets[next].position + time*_targets[next].velocity,
                _targets[next].velocity);

        list << _targets[next].position + time*_targets[next].velocity;

        for (unsigned int i(0), i_end(_targets.size()-1); i < i_end; ++i)
        {
            current = _solution[i];
            next = _solution[i+1];
            time += calculate_time(_velocity,
                    _targets[current].position + time*_targets[current].velocity,
                    _targets[next].position + time*_targets[next].velocity,
                    _targets[next].velocity);

            list << _targets[next].position + time*_targets[next].velocity;
        }
        // 'next' contains the last defined element at this point

        // calculate the time from the last target back to the origin
        time += calculate_time(
                _velocity,
                _targets[next].position + time*_targets[next].velocity,
                _origin.position + time*_origin.velocity,
                _origin.velocity);

        list << _origin.position + time*_origin.velocity;

        return list;
    }

    double getSolutionTime() const
    {
        return _solution.fitness();
    }

private:
    bool _done;
    const QList<TargetDataQt>& _targets; // TODO: dangerous
    const double _velocity;
    const TargetDataQt _origin;
    const double _start_time;
    bool _found_solution;
    MovingTargetTSP _solution;
    std::tuple<double, unsigned int, double, double> _saCoolingSchedule;
};

void ComputationThread::run()
{
    _stop = false;

    const std::array<double,3> p_and_v = {{ 0.0, 0.0, 0.0 }};
    const std::array<double,3> origin = {{ 0.0, 0.0, 0.0 }};

    // TODO: make computation base class and switch in start()
    if (_method == "Exhaustive Search")
    {
        ExhaustiveSearch es(_targets, _velocity, TargetDataQt(p_and_v, p_and_v, "origin"));

        while (!_stop && !es.done())
            es.step();

        if (es.done() && es.found_solution())
        {
            QList<std::array<double,3>> list;
            list << origin;
            for (long n(0); n < _targets.size(); ++n)
            {
                list << es.get_shortest_path_target_position(n);
            }
            list << origin;
            emit solutionFound(list);
        }
    }
    else if (_method == "Simulated Annealing")
    {
        SimulatedAnnealing sa(_targets, _velocity, TargetDataQt(p_and_v, p_and_v, "origin"), _currentSACoolingSchedule);
        sa.compute_all();
        emit solutionFound(sa.get_shortest_path_target_position());
        emit log(QString("computation finished, total duration of the tour: %1").arg(sa.getSolutionTime()));
    }
    else
    {
        emit log(QString("Unknown computation method selected: %1").arg(_method));
    }
}

void ComputationThread::start(const QList<TargetDataQt>& targets, double velocity, const QString& method)
{
    _targets = targets;
    _velocity = velocity;
    _method = method;
    emit log(QString("calculating tour for %1 targets").arg(_targets.size()));
    QThread::start();
}
void ComputationThread::stop()
{
    _stop = true;
}

