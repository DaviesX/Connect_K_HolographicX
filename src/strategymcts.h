#ifndef STRATEGYMCTS_H
#define STRATEGYMCTS_H

#include <math.h>
#include <vector>
#include "istrategy.h"

#define NUM_CACHE_NODES 5
#define CACHE_DEPTH     4

class StrategyMCTS : public IStrategy
{
public:
        StrategyMCTS();

        void    load_state(const State& s) override;
        void    make_move(const State& s, unsigned quality, unsigned time, Move& m) const override;
        void    print(std::ostream& os) const override;
};

#endif // STRATEGYMCTS_H
