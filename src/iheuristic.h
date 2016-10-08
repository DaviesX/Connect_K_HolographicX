#ifndef IHEURISTIC_H
#define IHEURISTIC_H

class State;

/*
 * <Heuristic> Path cost estimate to the optimal goal from some state n, (should be admissible).
 */
class IHeuristic
{
public:
        IHeuristic();
        float evaluate(State& s) const;
};


#endif  // IHEURISTIC_H
