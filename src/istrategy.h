#ifndef ISTRATEGY_H
#define ISTRATEGY_H

#include <ostream>

class State;
class Move;

/*
 * <IStrategy> Core of the agent.
 */
class IStrategy
{
public:
        IStrategy();
        virtual ~IStrategy();
        virtual void    load_state(const State& s) = 0;
        virtual void    make_move(const State& s, Move& m) = 0;
        virtual void    print(std::ostream& os) const;
};

std::ostream& operator<<(std::ostream& os, const IStrategy& s);


#endif  // ISTRATEGY_H
