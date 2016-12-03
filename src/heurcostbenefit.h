#ifndef HEUR_COST_BENEFIT_H
#define HEUR_COST_BENEFIT_H

#include <vector>
#include "iheuristic.h"

class HeuristicCostBenefit: public IHeuristic
{
public:
        HeuristicCostBenefit();
        ~HeuristicCostBenefit();
        void    try_move(const State& s, const Move& m) override;
        void    untry_move() override;
        float   coarse_eval(const State& s, const Move& next_move) const override;
        float   evaluate(const State& s, const Move& next_move) const override;

        // Link statistics.
        struct LinkStat
        {
                LinkStat(int who):
                        who(who)
                {
                        oppo = opponent_of(who);
                }

                LinkStat():
                        LinkStat(State::AI_PIECE)
                {
                }

                void reset()
                {
                        ins = 0;
                        del = 0;
                        state = 0;
                }

                int             who;
                unsigned        ins = 0;
                unsigned        state = 0;
                unsigned        del = 0;
                int             oppo;
        };

        // Exponent at each direction.
        struct Exp {
                Exp()
                {
                        *(unsigned*)exp = 0;
                }

                char exp[4];
        };
private:
        float   evaluate_move(const State& s, const Move& move, int who) const;
        float   benefit(const State& s, const Move& next_move, int who, int extra_moves) const;

        std::vector<Move>       m_stack;
};

#endif // HEUR_COST_BENEFIT_H
