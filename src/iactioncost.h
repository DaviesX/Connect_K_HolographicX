#ifndef IACTIONCOST_H
#define IACTIONCOST_H

class State;
class Move;

class IActionCost
{
public:
        IActionCost();
        virtual float evaluate(const State& s, const Move& m) = 0;
        virtual void print_dbg_info() const;
};

#endif  // IACTIONCOST_H
