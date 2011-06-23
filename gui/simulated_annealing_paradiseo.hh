/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef SIMULATED_ANNEALING_HH
#define SIMULATED_ANNEALING_HH

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
#include <continuator/moTrueContinuator.h>
#include <continuator/moCheckpoint.h>
#include <continuator/moFitnessStat.h>
#include <utils/eoFileMonitor.h>
#include <continuator/moCounterMonitorSaver.h>

/* represent our Moving Target TSP by a permutation,
 * the time duration of our tour will be the fitness
 * and the target is to minimize the time */
typedef eoInt<eoMinimizingFitness> MovingTargetTSP;

typedef moShiftNeighbor<MovingTargetTSP> ShiftNeighbor; //shift Neighbor
typedef moRndWithReplNeighborhood<ShiftNeighbor> RndShiftNeighborhood; //rnd shift Neighborhood (Indexed)

template< class Neighbor >
class moWatchBooleanContinuator :
    public moContinuator<Neighbor>
{
public:
    typedef typename Neighbor::EOT EOT;

    moWatchBooleanContinuator(const bool& var) :
        _continue(var)
    {
    }

    virtual bool operator()(EOT & _solution)
    {
        return _continue;
    }

private:
    const bool& _continue;
};

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

class SimulatedAnnealingParadiseo
{
public:
    SimulatedAnnealingParadiseo(const QList<TargetDataQt>& targets, double velocity, const TargetDataQt& origin, const std::tuple<double, unsigned int, double, double>& SACoolingSchedule, double start_time = 0.0) :
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
        moSolNeighborComparator<ShiftNeighbor> solComparator;

        moTrueContinuator<ShiftNeighbor> continuator;
        moCheckpoint<ShiftNeighbor> checkpoint(continuator);

        moFitnessStat<MovingTargetTSP> fitStat;
        checkpoint.add(fitStat);
        eoFileMonitor monitor("fitness.out", "");
        moCounterMonitorSaver countMon(100, monitor);
        checkpoint.add(countMon);
        monitor.add(fitStat);

        moSA<ShiftNeighbor> localSearch(rndShiftNH, fullEval, shiftEval, coolingSchedule, solComparator, checkpoint);

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


#endif // SIMULATED_ANNEALING_HH
