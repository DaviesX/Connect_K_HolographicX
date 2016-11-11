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
        virtual void    try_move(const State& s, const Move& m);
        virtual void    untry_move();
        virtual float   coarse_eval(const State& s, const Move& next_move) const;
        virtual float   evaluate(const State& s, const Move& next_move) const = 0;
        virtual bool    is_goal_for(const State& s, const Move& next_move, int who) const;
        virtual void    print(std::ostream& os) const;
};

std::ostream& operator<<(std::ostream& os, const IHeuristic& h);


#endif  // IHEURISTIC_H
