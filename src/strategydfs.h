#ifndef STRATEGYDFS
#define STRATEGYDFS

#include <istrategy.h>

class IActionCost;

class StrategyDFS: public IStrategy
{
public:
        StrategyDFS();
        ~StrategyDFS() override;

        IActionCost*    get_gxy() const override;
        IHeuristic*     get_fxy() const override;
        void            make_move(const State& s, Move& m) override;
};


#endif  // STRATEGYDFS
