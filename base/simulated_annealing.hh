/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef SIMULATED_ANNEALING_HH
#define SIMULATED_ANNEALING_HH

#include <vector>

#include <QtCore/QObject>

#include "base/target_data.hh"
#include "base/moving_target_tsp.hh"

class SimulatedAnnealing :
    public QObject
{
    Q_OBJECT

public:
    SimulatedAnnealing(QObject* p = 0);

    /**
     * set all data required for computation
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
    std::vector<std::pair<double, size_t>> getSolution() const;

    void setCoolingParameters(double initialTemperature, double finalTemperature, double decreaseCoefficient, size_t maxSameTemperatureSteps);
    void setCoolingParameters(unsigned int samplingSteps, double finalTemperature, double decreaseCoefficient, size_t maxSameTemperatureSteps);

public slots:
    /**
     * abort current calculations
     * only if calculations are running in a separate thread,
     * otherwise the signal won't be delivered, resp. you are unable
     * to call abort()
     */
    void abort();

    /**
     * evaluate/calculate tours
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
    double _startTime;
    bool _abort;
    bool _finished;

    // simulated annealing initial parameters
    double _initialTemperature;
    double _finalTemperature;
    double _decreaseCoefficient;
    size_t _maxSameTemperatureSteps;
    size_t _samplingSteps;

    // we need this value at various occasions
    size_t _targetCount;

    MovingTargetTSP _optimalTour;

    /**
     * helper function to decided whether a new calculated energy is acceptable based on the given temperature
     */
    static bool _isEnergyAcceptable(const double& e, const double& new_e, const double& temperature);

    static void _evaluateTime(MovingTargetTSP& mttsp);

    void _calculateTemperature();
};

#endif // SIMULATED_ANNEALING_HH
