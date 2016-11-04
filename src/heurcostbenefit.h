#ifndef HEUR_COST_BENEFIT_H
#define HEUR_COST_BENEFIT_H

#include <vector>
#include "iheuristic.h"

class HeuristicCostBenefit: public IHeuristic
{
public:
        HeuristicCostBenefit();
        void    try_move(const State& s, const Move& m) override;
        void    untry_move() override;
        float   evaluate(const State& s, const Move& next_move) const override;
private:
        float   evaluate_move(const State& s, const Move& move) const;
        float   cost(const State& s, const Move& next_move, int who) const;
        float   benefit(const State& s, const Move& next_move, int who, int extra_moves) const;

        unsigned*               m_exp_map = nullptr;
        unsigned                m_exp_w = 0;
        unsigned                m_exp_h = 0;

        std::vector<float>      m_stack;
        float                   m_path_score = 0;
};

#endif // HEUR_COST_BENEFIT_H
