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
        virtual void    load_state(const State& s) = 0; 
        virtual void    accept(const State& s, unsigned x, unsigned y) = 0;
        virtual float   evaluate(const State& s, unsigned x, unsigned y) = 0;
        virtual void    print(std::ostream& os) const;
};

std::ostream& operator<<(std::ostream& os, const IHeuristic& h);


#endif  // IHEURISTIC_H
