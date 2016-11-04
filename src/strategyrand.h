#ifndef STRATEGYRAND_H
#define STRATEGYRAND_H

#include "istrategy.h"


class StrategyRandom: public IStrategy
{
public:
        StrategyRandom();
        void    load_state(const State& s) override;
        void    make_move(const State& s, Move& m) const override;
};

#endif  // STRATEGYRAND_H
