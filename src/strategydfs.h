#ifndef STRATEGYDFS
#define STRATEGYDFS

#include <istrategy.h>

class IActionCost;

class StrategyDFS: public IStrategy
{
public:
        StrategyDFS();
        ~StrategyDFS() override;
        void make_move(const State& s, Move& m) override;
private:
        IActionCost*    m_fcost;
};


#endif  // STRATEGYDFS
