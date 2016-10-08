#ifndef IHEURISTIC_H
#define IHEURISTIC_H

class State;
class Move;

/*
 * <Heuristic> Path cost estimate to the optimal goal from some state n, (should be admissible).
 */
class IHeuristic
{
public:
        IHeuristic();
        virtual float evaluate(const State& s, const Move& m) const = 0;
};


#endif  // IHEURISTIC_H
