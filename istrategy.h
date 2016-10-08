#ifndef ISTRATEGY_H
#define ISTRATEGY_H

class State;
class Move;

/*
 * <IStrategy> Core of the agent.
 */
class IStrategy
{
public:
        IStrategy();
        virtual void make_move(const State& s, Move& m) = 0;
};

#endif  // ISTRATEGY_H
