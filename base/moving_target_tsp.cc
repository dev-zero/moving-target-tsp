/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include "base/moving_target_tsp.hh"
#include "utils/common_calculation_functions.hh"

#include <QtCore/QDebug>

#include <limits>
#include <algorithm>
#include <cassert>

MovingTargetTSP::MovingTargetTSP() :
    _targets(0)
{
}

MovingTargetTSP::MovingTargetTSP(const std::vector<TargetDataQt> * targets, size_t origin, double startTime, double velocity) :
    _startIndex(1),
    _targets(targets),
    _totalTime(std::numeric_limits<double>::infinity()),
    _startTime(startTime),
    _velocity(velocity)
{
    _path.resize(targets->size()+1);

    // filling the _permutation with indices
    size_t count(0);
    std::generate(_path.begin(), _path.end(), [&count]() { return std::make_pair(0.0, count++); });

    // path contains n+1 elements in ascending order and therefore can _origin directly be used as an index
    // _origin can also not target the last element since 0<=origin<numberOfTargets
    std::swap(_path.front(), _path[origin]);

    // and set the last index in the path to _origin as well
    _path.back().second = origin;

    // and update ourselves, such that a Move operator can assume _startIndex can always be overwritten
    update();
}

void MovingTargetTSP::dump() const
{
    qDebug() << "MovingTargetTSP instance" << this;
    qDebug() << "  _path:";
    for (auto i(_path.begin()), i_end(_path.end()); i != i_end; ++i)
        qDebug() << "    " << i->second << "@" << i->first;
    qDebug() << "  _startIndex:" << _startIndex;
    qDebug() << "  _totalTime:" << _totalTime;
}

bool MovingTargetTSP::isValid() const
{
    return _targets != 0;
}

void MovingTargetTSP::update()
{
    if (_startIndex < _path.size())
    {
        double time(_startTime);

        if (_startIndex > 1)
            time = _path[_startIndex-1].first;

        for (size_t i(_startIndex-1); i < _path.size()-1; ++i)
        {
            _path[i].first = time;
            time += calculate_time(
                    _velocity,
                    (*_targets)[_path[i].second].position   + time * (*_targets)[_path[i].second].velocity,
                    (*_targets)[_path[i+1].second].position + time * (*_targets)[_path[i+1].second].velocity,
                    (*_targets)[_path[i+1].second].velocity);
        }
        _path.back().first = time;
        _totalTime = time;
        _startIndex = _path.size();
    }
}

double MovingTargetTSP::totalTime() const
{
    assert(_startIndex == _path.size());
    return _totalTime;
}

bool MovingTargetTSP::isSolution() const
{
    return _totalTime < std::numeric_limits<double>::infinity();
}

MovingTargetTSP::Permutation MovingTargetTSP::path() const
{
    return _path;
}

