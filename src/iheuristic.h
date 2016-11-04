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
        virtual void    try_move(const Move& m, int who);
        virtual void    untry_move(const Move& m, int who);
        virtual float   evaluate(const State& s, const Move& next_move) const = 0;
        virtual void    print(std::ostream& os) const;
};

std::ostream& operator<<(std::ostream& os, const IHeuristic& h);


#endif  // IHEURISTIC_H
