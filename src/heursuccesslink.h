#ifndef HEUR_SUCCESS_LINK_H
#define HEUR_SUCCESS_LINK_H

#include <iheuristic.h>

class HeuristicSuccessLink: public IHeuristic
{
public:
        float   evaluate(const State& s, const Move& next_move, int who) override;
};


#endif
