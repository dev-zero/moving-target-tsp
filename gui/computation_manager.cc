/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */


#include "gui/computation_manager.hh"
#include "gui/exhaustive_search_recursive.hh"
#include "utils/common_calculation_functions.hh"

#include <QtCore/QThread>
#include <QtCore/QDebug>

ComputationManager::ComputationManager(QObject* p) :
    QObject(p)
{
    _thread = new QThread(this);

    _exhaustiveSearch = new ExhaustiveSearchRecursive;
    _exhaustiveSearch->moveToThread(_thread);

    connect(_exhaustiveSearch, SIGNAL(finished()), SLOT(_algorithmFinished()));

    _thread->start();
}

ComputationManager::~ComputationManager()
{
    _exhaustiveSearch->abort();
    _thread->quit();
    _thread->wait();
}

void ComputationManager::switchAlgorithm(const QString& identifier)
{
    // ignoring for now
}

void ComputationManager::setParameters(const QString& identifier, const QMap<QString, QVariant>& parameters)
{
    // ignoring for now
}

void ComputationManager::setData(const QList<TargetDataQt>& targets, double velocity)
{
    _targets.clear();
    _targets.reserve(targets.size()+1);
    qCopy(targets.constBegin(), targets.constEnd(), std::back_inserter(_targets));

    const std::array<double,3> origin = {{ 0.0, 0.0, 0.0 }};
    _targets.push_back(TargetDataQt(origin, origin, "origin"));

    _exhaustiveSearch->set(&_targets, velocity, _targets.size()-1);
}

void ComputationManager::start()
{
    QMetaObject::invokeMethod(_exhaustiveSearch, "evaluate", Qt::QueuedConnection);
    emit started();
}

void ComputationManager::abort()
{
    _exhaustiveSearch->abort();
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
    if (_exhaustiveSearch->solutionFound())
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
    else
        qDebug() << "no solution found";

    emit finished();
}
