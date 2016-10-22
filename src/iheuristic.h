#ifndef IHEURISTIC_H
#define IHEURISTIC_H


#include <ostream>

class State;
class Move;

/*
 * <Heuristic> Path cost estimate to the optimal goal from some state n, (should be admissible).
 */
class IHeuristic
{
public:
        IHeuristic();
        virtual ~IHeuristic();
        virtual void    load_state(const State& s);
        virtual void    accept(const Move& m, int who, float score);
        virtual float   evaluate(const State& s, const Move& next_move, int who) = 0;
        virtual void    print(std::ostream& os) const;
};

std::ostream& operator<<(std::ostream& os, const IHeuristic& h);


#endif  // IHEURISTIC_H
