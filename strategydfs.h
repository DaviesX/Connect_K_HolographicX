#ifndef STRATEGYDFS
#define STRATEGYDFS

#include <istrategy.h>

class StrategyDFS: public IStrategy
{
public:
        StrategyDFS();
        void make_move(const State& s, Move& m) override;
};


#endif  // STRATEGYDFS
