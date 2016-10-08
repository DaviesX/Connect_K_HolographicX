#include <strategyrand.h>
#include <strategydfs.h>
#include <strategyfactory.h>


IStrategy* StrategyFactory::create(StrategyType s) const
{
        switch (s) {
                case Random:
                        return new StrategyRandom();
                case DFS:
                        return new StrategyDFS();
                default:
                        return nullptr;
        };
}
