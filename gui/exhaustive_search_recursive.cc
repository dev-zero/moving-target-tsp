/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include "gui/exhaustive_search_recursive.hh"
#include "utils/common_calculation_functions.hh"

ExhaustiveSearchRecursive::ExhaustiveSearchRecursive(QObject* p) :
    QObject(p),
    _abort(true),
    _finished(false)
{
}

void ExhaustiveSearchRecursive::set(std::vector<TargetDataQt> const * targets, double velocity, size_t origin, double start_time)
{
    _targets = targets;
    _velocity = velocity;
    _origin = origin;
    _start_time = start_time;
    _best_time = std::numeric_limits<double>::infinity();
    _abort = false;
    _finished = false;
}

void ExhaustiveSearchRecursive::evaluate()
{
    std::vector<size_t> indices(_targets->size());

    size_t count(0);
    std::generate(indices.begin(), indices.end(), [&count]() { return count++; });
    
    indices.erase(indices.begin() + _origin);

    _calculate_subtour(_origin, _start_time, indices);

    _finished = true;
    emit finished();
}

bool ExhaustiveSearchRecursive::solutionFound() const
{
    return _finished && !_abort && (_optimal_tour.size() > 0);
}

void ExhaustiveSearchRecursive::_calculate_subtour(size_t start, double time, const std::vector<size_t>& indices)
{
    if (_abort)
        return;

    _current_tour.push_back(std::make_pair(time, start));
    // if indices is empty, start is the last target before returning to the origin,
    // thus calculate the time for returning to the origin
    if (indices.size() == 0)
    {
        double cur_time = time + calculate_time(
                _velocity,
                (*_targets)[start].position + time * (*_targets)[start].velocity,
                (*_targets)[_origin].position + time * (*_targets)[_origin].velocity,
                (*_targets)[_origin].velocity);

        _current_tour.push_back(std::make_pair(cur_time, _origin));

        if (cur_time < _best_time)
        {
            _best_time = cur_time;
            _optimal_tour = _current_tour;
        }

        _current_tour.pop_back();
        _current_tour.pop_back();
        return;
    }

    for (size_t i(0); i < indices.size(); ++i)
    {
        double new_time(time);
        new_time += calculate_time(
                _velocity,
                (*_targets)[start].position + time * (*_targets)[start].velocity,
                (*_targets)[indices[i]].position + time * (*_targets)[indices[i]].velocity,
                (*_targets)[indices[i]].velocity);


        std::vector<size_t> new_indices(indices);
        new_indices.erase(new_indices.begin() + i);

        _calculate_subtour(indices[i], new_time, new_indices);
    }

    _current_tour.pop_back();
    return;
}

void ExhaustiveSearchRecursive::abort()
{
    _abort = true;
}

std::deque<std::pair<double, size_t>> ExhaustiveSearchRecursive::getSolution() const
{
    return _optimal_tour;
}

