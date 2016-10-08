#include <strategydfs.h>
#include <actioncostlink.h>


StrategyDFS::StrategyDFS()
{
        m_fcost = new ActionCostLink();
}

StrategyDFS::~StrategyDFS()
{
}

void StrategyDFS::make_move(const State& s, Move& m)
{
}
