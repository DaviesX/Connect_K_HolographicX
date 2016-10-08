#ifndef IACTIONCOST_H
#define IACTIONCOST_H

class State;
class Move;

class IActionCost
{
public:
        IActionCost();
        virtual float evaluate(const State& s, const Move& m) const = 0;
};

#endif  // IACTIONCOST_H
