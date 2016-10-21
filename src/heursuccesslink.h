#ifndef HEUR_SUCCESS_LINK_H
#define HEUR_SUCCESS_LINK_H

#include <iheuristic.h>

class HeuristicSuccessLink: public IHeuristic
{
public:
        void    load_state(const State& s) override;
        void    accept(const Move& m, int who, float score) override;
        float   evaluate(const State& s, const Move& next_move, int who) override;
private:
        float   p0;
        float   p1;
};


#endif
