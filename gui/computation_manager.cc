/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */


#include "gui/computation_manager.hh"
#include "gui/exhaustive_search_recursive.hh"
#include "gui/simulated_annealing.hh"
#include "utils/common_calculation_functions.hh"

#include <QtCore/QThread>
#include <QtCore/QDebug>

ComputationManager::ComputationManager(QObject* p) :
    QObject(p),
    _selectedAlgorithm("Exhaustive Search")
{
    _thread = new QThread(this);

    _exhaustiveSearch = new ExhaustiveSearchRecursive;
    _simulatedAnnealing = new SimulatedAnnealing;

    _exhaustiveSearch->moveToThread(_thread);
    _simulatedAnnealing->moveToThread(_thread);

    connect(_exhaustiveSearch, SIGNAL(finished()), SLOT(_algorithmFinished()));
    connect(_simulatedAnnealing, SIGNAL(finished()), SLOT(_algorithmFinished()));

    _thread->start();
}

ComputationManager::~ComputationManager()
{
    _exhaustiveSearch->abort();
    _simulatedAnnealing->abort();
    _thread->quit();
    _thread->wait();
}

void ComputationManager::switchAlgorithm(const QString& identifier)
{
    _selectedAlgorithm = identifier;
}

void ComputationManager::setParameters(const QString& identifier, const QMap<QString, QVariant>& parameters)
{
    if (identifier == "Simulated Annealing")
    {
        _simulatedAnnealing->setCoolingParameters(
                parameters["initialTemperature"].toDouble(),
                parameters["finalTemperature"].toDouble(),
                parameters["decreaseCoefficient"].toDouble(),
                parameters["maxSameTemperatureSteps"].toUInt());
    }
}

void ComputationManager::setData(const QList<TargetDataQt>& targets, double velocity)
{
    _targets.clear();
    _targets.reserve(targets.size()+1);
    qCopy(targets.constBegin(), targets.constEnd(), std::back_inserter(_targets));

    const std::array<double,3> origin = {{ 0.0, 0.0, 0.0 }};
    _targets.push_back(TargetDataQt(origin, origin, "origin"));

    if (_selectedAlgorithm == "Exhaustive Search")
        _exhaustiveSearch->set(&_targets, velocity, _targets.size()-1);
    else if (_selectedAlgorithm == "Simulated Annealing")
        _simulatedAnnealing->set(&_targets, velocity, _targets.size()-1);
}

void ComputationManager::start()
{
    if (_selectedAlgorithm == "Exhaustive Search")
        QMetaObject::invokeMethod(_exhaustiveSearch, "evaluate", Qt::QueuedConnection);
    else if (_selectedAlgorithm == "Simulated Annealing")
        QMetaObject::invokeMethod(_simulatedAnnealing, "evaluate", Qt::QueuedConnection);

    emit started();
}

void ComputationManager::abort()
{
    _exhaustiveSearch->abort();
    _simulatedAnnealing->abort();
}

double _tourLength(const QList<std::array<double,3>>& list)
{
    double length(0.0);

    for (QList<std::array<double,3>>::size_type i(0), i_end(list.count()-1); i < i_end; ++i)
        length += norm( list.at(i+1) - list.at(i) );

    return length;
}

void ComputationManager::_algorithmFinished()
{
    if (_selectedAlgorithm == "Exhaustive Search" && _exhaustiveSearch->solutionFound())
    {
        std::deque<std::pair<double, size_t>> solution(_exhaustiveSearch->getSolution());

        QList<std::array<double,3>> positions;

        for (auto i(solution.begin()); i != solution.end(); ++i)
        {
            const TargetDataQt& target(_targets[i->second]);
            const double time(i->first);
            positions << (target.position + time*target.velocity);
        }
        emit solutionFound(positions, solution.back().first, _tourLength(positions));
    }
    else if (_selectedAlgorithm == "Simulated Annealing" && _simulatedAnnealing->solutionFound())
    {
        std::vector<std::pair<double, size_t>> solution(_simulatedAnnealing->getSolution());

        QList<std::array<double,3>> positions;

        for (auto i(solution.begin()); i != solution.end(); ++i)
        {
            const TargetDataQt& target(_targets[i->second]);
            const double time(i->first);
            positions << (target.position + time*target.velocity);
        }
        emit solutionFound(positions, solution.back().first, _tourLength(positions));

    }
    else
        qDebug() << "no solution found";

    emit finished();
}
