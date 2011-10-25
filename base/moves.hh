/* vim: set sw=4 sts=4 ft=cpp et foldmethod=syntax : */

/*
 * Copyright (c) 2011 Tiziano Müller <tm@dev-zero.ch>
 *
 *
 *
 */

#ifndef MOVES_HH
#define MOVES_HH

#include <algorithm>
#include <cmath>
#include <cstdlib>

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

#endif // MOVES_HH
