#ifndef HEUR_COST_BENEFIT_H
#define HEUR_COST_BENEFIT_H

#include "iheuristic.h"

class HeuristicCostBenefit: public IHeuristic
{
public:
        HeuristicCostBenefit();
        float   evaluate(const State& s, const Move& next_move) const override;
private:
        float cost(const State& s, const Move& next_move, int who) const;
        float benefit(const State& s, const Move& next_move, int who, int extra_moves) const;
};

#endif // HEUR_COST_BENEFIT_H
