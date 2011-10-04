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
#include <cmath>

#include <QtCore/QObject>

#include "gui/target_data.hh"

/**
 * an abstract move operating on a sequence
 */
template<typename ContainerT>
struct Move
{
    typedef typename ContainerT::iterator ContainerIterT;

    /**
     * The move operator, to be reimplemented in a derived class
     * @param begin Iterator for the first element in the range
     * @param end Iterator pointing to one element after the last to include in the range
     * @return index of the first changed element
     */
    virtual size_t operator()(ContainerIterT begin, ContainerIterT end) = 0;
};

/**
 * swaps two neighbour elements in a sequence
 */
template<typename ContainerT>
struct RandomNeighbourSwapMove :
    public Move<ContainerT>
{
    typedef typename ContainerT::iterator ContainerIterT;
    size_t operator()(ContainerIterT begin, ContainerIterT end)
    {
        size_t size(std::distance(begin, end));
        // generate random number r: 1 <= r <= size
        size_t n(ceil( (rand()/(RAND_MAX + 1.0)) * size ));
        size_t i(n % size), j((n+1) % size);
        std::swap(*(begin + i), *(begin + j));
        return std::min(i, j);
    }
};

/**
 * completely random shuffle of the permutation
 */
template<typename ContainerT>
struct RandomMove :
    public Move<ContainerT>
{
    typedef typename ContainerT::iterator ContainerIterT;
    size_t operator()(ContainerIterT begin, ContainerIterT end)
    {
        size_t size(std::distance(begin, end));

        for (size_t i(0); i < size; ++i)
        {
            // generate random number r: 0 <= r < (size-i)
            size_t n(floor( (rand()/(RAND_MAX + 1.0)) * (size-i) ));
            std::swap(*(begin + i), *(begin + i + n));
        }
        return 0;
    }
};

template<typename MoveT> class MovingTargetTSPMove;

class MovingTargetTSP
{
public:
    typedef std::vector<std::pair<double, size_t>> Permutation;

    MovingTargetTSP();

    MovingTargetTSP(const std::vector<TargetDataQt> * targets, size_t origin, double startTime, double velocity);

    void update();

    double totalTime() const;

    bool isSolution() const;

    Permutation path() const;

    void dump() const;

    bool isValid() const;

protected:
    friend class MovingTargetTSPMove<RandomNeighbourSwapMove<Permutation>>;
    friend class MovingTargetTSPMove<RandomMove<Permutation>>;

    Permutation _path;
    size_t _startIndex;

private:
    const std::vector<TargetDataQt> * _targets;
    double _totalTime;
    double _startTime;
    double _velocity;
};

template<typename MoveT>
class MovingTargetTSPMove
{
private:
    MoveT move;
public:
    void operator()(MovingTargetTSP& mttsp)
    {
        mttsp._startIndex = move(mttsp._path.begin()+1, mttsp._path.end()-1) + 1;
    }
};


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
