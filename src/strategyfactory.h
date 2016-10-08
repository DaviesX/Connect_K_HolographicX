#ifndef STRATEGYFACTORY_H
#define STRATEGYFACTORY_H

class IStrategy;

class StrategyFactory
{
public:
        enum StrategyType {
                Random,
                DFS,
        };
        
        IStrategy*      create(StrategyType type) const;
};


#endif  // STRATEGYFACTORY_H
