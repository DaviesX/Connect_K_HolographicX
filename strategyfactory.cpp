#include <state.h>
#include <strategyrand.h>
#include <strategydfs.h>
#include <strategyfactory.h>


IStrategy* StrategyFactory::create(StrategyType type, const State& s) const
{
        switch (type) {
                case Random:
                        return new StrategyRandom();
                case DFS:
                        return new StrategyDFS();
                default:
                        return nullptr;
        };
}
