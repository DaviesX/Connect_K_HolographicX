#ifndef HEUR_SUCCESS_LINK_H
#define HEUR_SUCCESS_LINK_H

#include <iheuristic.h>

class HeuristicSuccessLink: public IHeuristic
{
public:
        void    load_state(const State& s) override;
        float   evaluate(const State& s, const Move& next_move) override;
};


#endif
