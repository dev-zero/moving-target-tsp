/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include "utils/common_calculation_functions.hh"
#include "gui/computation_thread.hh"
#include "gui/exhaustive_search.hh"
#include "gui/simulated_annealing.hh"
#include "gui/target_data.hh"


double tourLength(const QList<std::array<double,3>>& list)
{
    double length(0.0);
    for (QList<std::array<double,3>>::size_type i(0), i_end(list.count()-1); i < i_end; ++i)
    {
        length += sqrt( inner_prod( list.at(i+1)-list.at(i), list.at(i+1)-list.at(i) ) );
    }
    length += sqrt( inner_prod( list.last()-list.first(), list.last()-list.first() ) );
    return length;
}

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

            emit solutionFound(list, es.getSolutionTime(), tourLength(list));
        }
    }
    else if (_method == "Simulated Annealing")
    {
        SimulatedAnnealing sa(_targets, _velocity, TargetDataQt(p_and_v, p_and_v, "origin"), _currentSACoolingSchedule);
        emit log(QString("starting computing with initialT=%1 decreaseFactor=%2 equalSteps=%3 finalT=%4")
                .arg(std::get<0>(_currentSACoolingSchedule))
                .arg(std::get<1>(_currentSACoolingSchedule))
                .arg(std::get<2>(_currentSACoolingSchedule))
                .arg(std::get<3>(_currentSACoolingSchedule)));
        sa.compute_all();
        QList<std::array<double,3>> list(sa.get_shortest_path_target_position());
        emit solutionFound(list, sa.getSolutionTime(), tourLength(list));
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

