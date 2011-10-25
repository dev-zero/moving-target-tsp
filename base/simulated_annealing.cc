/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#include "base/simulated_annealing.hh"
#include "utils/common_calculation_functions.hh"

#include <QtCore/QDebug>

#include <limits>
#include <numeric>

SimulatedAnnealing::SimulatedAnnealing(QObject* p) :
    QObject(p),
    _abort(true),
    _finished(false),
    _initialTemperature(10.0),
    _finalTemperature(0.01),
    _decreaseCoefficient(0.9),
    _maxSameTemperatureSteps(1000),
    _samplingSteps(100)
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
    return _finished && _optimalTour.isValid();
}

std::vector<std::pair<double, size_t>> SimulatedAnnealing::getSolution() const
{
    return _optimalTour.path();
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

    size_t sameSteps() const
    {
        return _maxSameTemperatureSteps - _currentStep;
    }

    void update(double& temperature, size_t seek = 1)
    {
        _currentStep += seek;

        if (_currentStep >= _maxSameTemperatureSteps)
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

    size_t _currentStep;
};

void SimulatedAnnealing::_calculateTemperature()
{
    qDebug() << "calculating the initial temperature based on" << _samplingSteps << "steps";

    MovingTargetTSPMove<RandomMove<MovingTargetTSP::Permutation>> move;
    std::vector<double> results(_samplingSteps);

    // collect samples
    MovingTargetTSP mttsp(_targets, _origin, _startTime, _velocity);
    size_t samplingRuns(0);
    for (size_t collectedSamples(0); collectedSamples < _samplingSteps;)
    {
        move(mttsp);
        mttsp.update();
        if (mttsp.totalTime() < std::numeric_limits<double>::infinity())
        {
            results[collectedSamples] = mttsp.totalTime();
            ++collectedSamples;
        }

        if (samplingRuns++ > _samplingSteps*100)
        {
            qDebug() << "could not generate requested number of samples in" << _samplingSteps*100 << "steps";
            _abort = true;
        }

        if (_abort)
            return;

    }
    double mean = std::accumulate(results.begin(), results.end(), 0.0) / results.size();

    /*
       double sigma = sqrt(std::accumulate(results.begin(), results.end(), 0.0, 
       [&mean](double sum, double value) { return sum + (value-mean)*(value-mean); })/results.size());

       p = 3*sigma;

       double k = -3.0 / ln(p);

       _initialTemperature = sigma * k;
       */

    _initialTemperature = 5 * mean;
}

void SimulatedAnnealing::evaluate()
{
    _optimalTour = MovingTargetTSP();

    if (_samplingSteps > 0)
    {
        _calculateTemperature();
        if (_abort)
        {
            qDebug() << "abort while calculating the initial temperature";
            emit finished();
            return;
        }
    }
    
    qDebug() << "starting new SA evaluation with the following parameters:";
    qDebug() << "  initial temperature: " << _initialTemperature;
    qDebug() << "  decrease coefficient:" << _decreaseCoefficient;
    qDebug() << "  max same T steps:    " << _maxSameTemperatureSteps;
    qDebug() << "  final temperature:   " << _finalTemperature;

    CoolingSchedule cooling(_initialTemperature, _decreaseCoefficient, _maxSameTemperatureSteps, _finalTemperature);

    double temperature(_initialTemperature);

    MovingTargetTSP solution(_targets, _origin, _startTime, _velocity), new_solution;
    MovingTargetTSPMove<RandomNeighbourSwapMove<MovingTargetTSP::Permutation>> move;
    MovingTargetTSPMove<RandomMove<MovingTargetTSP::Permutation>> randomMove;

    // shake the initial solution a little
    randomMove(solution);
    solution.update();

    double best_energy(std::numeric_limits<double>::infinity()), new_energy;

    do
    {
        new_solution = solution;

        move(new_solution);
        new_solution.update();
        new_energy = new_solution.totalTime();

        if (_isEnergyAcceptable(best_energy, new_energy, temperature))
        {
            _optimalTour = solution = new_solution;
            best_energy = new_energy;
        }

        cooling.update(temperature);

    } while (!_abort && cooling.proceed(temperature));

    _finished = true;
    emit finished();
}

void SimulatedAnnealing::_evaluateTime(MovingTargetTSP& mttsp)
{
    mttsp.update();
}

bool SimulatedAnnealing::_isEnergyAcceptable(const double& e, const double& new_e, const double& temperature)
{
    if (! (new_e < std::numeric_limits<double>::infinity()))
        return false;

    if (new_e < e)
        return true;

    double alpha = new_e < e ? 1.0 : exp((e-new_e)/temperature);
    return alpha > (rand()/(RAND_MAX + 1.0));
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
    _samplingSteps = 0;
}

void SimulatedAnnealing::setCoolingParameters(unsigned int samplingSteps, double finalTemperature, double decreaseCoefficient, size_t maxSameTemperatureSteps)
{
    qDebug() << "set new cooling parameters";
    qDebug() << "    " << samplingSteps;
    qDebug() << "    " << finalTemperature;
    qDebug() << "    " << decreaseCoefficient;
    qDebug() << "    " << maxSameTemperatureSteps;

    _samplingSteps = samplingSteps;
    _finalTemperature = finalTemperature;
    _decreaseCoefficient = decreaseCoefficient;
    _maxSameTemperatureSteps = maxSameTemperatureSteps;
}
