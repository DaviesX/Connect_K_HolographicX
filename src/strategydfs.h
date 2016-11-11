#ifndef STRATEGYDFS
#define STRATEGYDFS

#include <vector>
#include "stopwatch.h"
#include "iheuristic.h"
#include "heurchessdeg.h"
#include "heurcostbenefit.h"
#include "istrategy.h"


class StrategyDFS: public IStrategy
{
public:
        StrategyDFS();
        ~StrategyDFS() override;

        void            make_move(const State& s, unsigned quality, unsigned time, Move& m) const override;
        void            print_analysis(std::ostream& os, const State& s, int depth) const;
        void            print_analysis(std::ostream& os, const State& k, int depth, unsigned x, unsigned y) const;
};


#endif  // STRATEGYDFS
