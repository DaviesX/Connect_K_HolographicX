#ifndef HEUR_CHESS_DEG_H
#define HEUR_CHESS_DEG_H

#include "iheuristic.h"

class HeuristicChessDegree: public IHeuristic
{
public:
        float   evaluate(const State& s, const Move& next_move) const override;
};


#endif
