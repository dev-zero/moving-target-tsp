/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef MOVING_TARGET_TSP_HH
#define MOVING_TARGET_TSP_HH

#include <vector>

#include "base/target_data.hh"
#include "base/moves.hh"

template<typename MoveT> class MovingTargetTSPMove;

/**
 * encapsulates the Moving-Target TSP problem
 */
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

#endif // MOVING_TARGET_TSP_HH
