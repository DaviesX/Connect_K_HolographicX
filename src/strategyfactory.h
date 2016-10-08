#ifndef STRATEGYFACTORY_H
#define STRATEGYFACTORY_H

class IStrategy;
class State;

class StrategyFactory
{
public:
        enum StrategyType {
                Random,
                DFS,
        };
        
        IStrategy*      create(StrategyType type, const State& s) const;
};


#endif  // STRATEGYFACTORY_H
