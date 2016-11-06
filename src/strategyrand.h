#ifndef STRATEGYRAND_H
#define STRATEGYRAND_H

#include "istrategy.h"


class StrategyRandom: public IStrategy
{
public:
        StrategyRandom();
        void    load_state(const State& s) override;
        void    make_move(const State& s, unsigned quality, unsigned time, Move& m) const override;
};

#endif  // STRATEGYRAND_H
