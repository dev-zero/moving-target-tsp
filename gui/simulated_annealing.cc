/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include "gui/simulated_annealing.hh"
#include "utils/common_calculation_functions.hh"

#include <QtCore/QDebug>

#include <limits>
#include <cstdlib>

SimulatedAnnealing::SimulatedAnnealing(QObject* p) :
    QObject(p),
    _abort(true),
    _finished(false),
    _initialTemperature(10.0),
    _finalTemperature(0.01),
    _decreaseCoefficient(0.9),
    _maxSameTemperatureSteps(10)
{
}

void SimulatedAnnealing::set(std::vector<TargetDataQt> const * targets, double velocity, size_t origin, double startTime)
{
    _targets = targets;
    _velocity = velocity;
    _origin = origin;
    _startTime = startTime;
    _abort = false;
    _finished = false;

    _targetCount = targets->size()-1; // targets contains the origin
}

bool SimulatedAnnealing::solutionFound() const
{
    return _finished && !_abort && (_optimal_tour.size() > 0);
}

std::vector<std::pair<double, size_t>> SimulatedAnnealing::getSolution() const
{
    return _optimal_tour;
}

void SimulatedAnnealing::abort()
{
    _abort = true;
}

class CoolingSchedule
{
public:
    CoolingSchedule(const double& initialTemperature, const double& decreaseCoefficient, const size_t& maxSameTemperatureSteps, const double& finalTemperature) :
        _initialTemperature(initialTemperature),
        _decreaseCoefficient(decreaseCoefficient),
        _maxSameTemperatureSteps(maxSameTemperatureSteps),
        _finalTemperature(finalTemperature),
        _currentStep(0)
    {
    }

    void update(double& temperature)
    {
        if (_currentStep < _maxSameTemperatureSteps)
            ++_currentStep;
        else
        {
            temperature *= _decreaseCoefficient;
            _currentStep = 0;
        }
    }

    bool proceed(const double& temperature) const
    {
        return temperature > _finalTemperature;
    }
private:
    const double _initialTemperature;
    const double _decreaseCoefficient;
    const size_t _maxSameTemperatureSteps;
    const double _finalTemperature;

    double _currentStep;
};

void _dumpPath(const std::vector<std::pair<double, size_t>>& path)
{
    qDebug() << "path:";
    for (auto i(path.begin()), i_end(path.end()); i != i_end; ++i)
        qDebug() << "    " << i->second;
}

void SimulatedAnnealing::evaluate()
{
    _optimal_tour.clear();

    CoolingSchedule cooling(_initialTemperature, _decreaseCoefficient, _maxSameTemperatureSteps, _finalTemperature);
    double temperature(_initialTemperature);

    // _targets contains all targets including the origin (of which the index should be in _origin)
    // the path should contain the _origin twice: as start and as end point
    std::vector<std::pair<double, size_t>> path(_targetCount+2), new_path(_targetCount+2);

//    qDebug() << "number of targets (without origin): " << _targetCount << ", path size: " << path.size();

    // filling the path with indices
    size_t count(0);
    std::generate(path.begin(), path.end(), [&count]() { return std::make_pair(0.0, count++); });
    
    // path contains n+1 elements in ascending order and therefore can _origin directly be used as an index
    // _origin can also not target the last element since 0<=origin<numberOfTargets
    std::swap(path.front(), path[_origin]);

    // and set the last index in the path to _origin as well
    path.back().second = _origin;

//    _dumpPath(path);

    double best_energy(std::numeric_limits<double>::infinity()), new_energy;

    size_t firstChangedIndex(0);

    do
    {
        new_path = path;

        firstChangedIndex = _randomTwoOptMove(new_path);
        new_energy = _calculatePathEnergy(new_path, firstChangedIndex);

        if (_isEnergyAcceptable(best_energy, new_energy, temperature))
        {
            path = new_path;
            best_energy = new_energy;
        }

        cooling.update(temperature);

    } while (!_abort && cooling.proceed(temperature));

    // in contrary to the classic TSP a randomly chosen tour is not necessarily valid
    if (best_energy < std::numeric_limits<double>::infinity())
        _optimal_tour = path;

    _finished = true;
    emit finished();
}

size_t SimulatedAnnealing::_randomTwoOptMove(std::vector<std::pair<double, size_t>>& path) const
{
    size_t n(ceil( (rand()/(RAND_MAX + 1.0)) * _targetCount ));
    size_t i( (n % _targetCount) + 1), j( ((n+1) % _targetCount) + 1);
//    qDebug() << "twoOptMove switches targets at positions " << i << " (" << path[i].second << ") and " << j << " (" << path[j].second << ")";
    std::swap(path[i], path[j]);
    return std::min(i, j);
}

bool SimulatedAnnealing::_isEnergyAcceptable(const double& e, const double& new_e, const double& temperature) const
{
    if (new_e < e)
        return true;

    double alpha = new_e < e ? 1.0 : exp((e-new_e)/temperature);
    return alpha > (rand()/(RAND_MAX + 1.0));
}

double SimulatedAnnealing::_calculatePathEnergy(std::vector<std::pair<double, size_t>>& path, size_t startIndex) const
{
//    qDebug() << "calculatePathEnery(path, startIndex=" << startIndex << ")";
    assert(startIndex > 0 && "startIndex can not be 0");

    double time(_startTime);

    if (startIndex > 2)
        time = path[startIndex-1].first;

    for (size_t i(startIndex-1); i < path.size()-1; ++i)
    {
        path[i].first = time;
        time += calculate_time(
                _velocity,
                (*_targets)[path[i].second].position   + time * (*_targets)[path[i].second].velocity,
                (*_targets)[path[i+1].second].position + time * (*_targets)[path[i+1].second].velocity,
                (*_targets)[path[i+1].second].velocity);
    }
    path.back().first = time;
    return time;
}

void SimulatedAnnealing::setCoolingParameters(double initialTemperature, double finalTemperature, double decreaseCoefficient, size_t maxSameTemperatureSteps)
{
    qDebug() << "set new cooling parameters";
    qDebug() << "    " << initialTemperature;
    qDebug() << "    " << finalTemperature;
    qDebug() << "    " << decreaseCoefficient;
    qDebug() << "    " << maxSameTemperatureSteps;

    _initialTemperature = initialTemperature;
    _finalTemperature = finalTemperature;
    _decreaseCoefficient = decreaseCoefficient;
    _maxSameTemperatureSteps = maxSameTemperatureSteps;
}


