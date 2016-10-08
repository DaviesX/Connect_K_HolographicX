#ifndef ISTRATEGY_H
#define ISTRATEGY_H

class State;
class Move;
class IActionCost;
class IHeuristic;

/*
 * <IStrategy> Core of the agent.
 */
class IStrategy
{
public:
        IStrategy();
        virtual ~IStrategy();
        virtual IActionCost*    get_gxy() const;
        virtual IHeuristic*     get_fxy() const;
        virtual void            make_move(const State& s, Move& m) = 0;
};

#endif  // ISTRATEGY_H
