#ifndef STRATEGYRAND_H
#define STRATEGYRAND_H

#include <istrategy.h>


class StrategyRandom: public IStrategy
{
public:
        StrategyRandom();
        void make_move(const State& s, Move& m) override;
};

#endif  // STRATEGYRAND_H
