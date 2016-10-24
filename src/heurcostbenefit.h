#ifndef HEUR_COST_BENEFIT_H
#define HEUR_COST_BENEFIT_H

#include <iheuristic.h>

class HeuristicCostBenefit: public IHeuristic
{
public:
        HeuristicCostBenefit();
        float   evaluate(const State& s, const Move& next_move, int who) const override;
};

#endif // HEUR_COST_BENEFIT_H
