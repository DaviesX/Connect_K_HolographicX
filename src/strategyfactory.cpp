#include "state.h"
#include "strategyrand.h"
#include "strategydfs.h"
#include "strategymcts.h"
#include "strategyfactory.h"


IStrategy* StrategyFactory::create(StrategyType type) const
{
        switch (type) {
                case Random:
                        return new StrategyRandom();
                case DFS:
                        return new StrategyDFS();
                case MCTS:
                        return new StrategyMCTS();
                default:
                        return nullptr;
        };
}
