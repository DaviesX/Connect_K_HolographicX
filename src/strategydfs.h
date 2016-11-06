#ifndef STRATEGYDFS
#define STRATEGYDFS

#include <vector>
#include "stopwatch.h"
#include "iheuristic.h"
#include "heurchessdeg.h"
#include "heurcostbenefit.h"
#include "istrategy.h"


class StrategyDFS: public IStrategy
{
public:
        StrategyDFS();
        ~StrategyDFS() override;

        void            load_state(const State& s) override;
        void            make_move(const State& s, unsigned quality, unsigned time, Move& m) const override;
        void            print_analysis(std::ostream& os, const State& s, int depth) const;
        void            print_analysis(std::ostream& os, const State& k, int depth, unsigned x, unsigned y) const;
private:
        struct AvailableAction
        {
                AvailableAction(int x, int y, int score):
                        x(x), y(y), score(score)
                {
                }

                bool operator < (const AvailableAction& action) const
                {
                        return score < action.score;
                }

                bool operator > (const AvailableAction& action) const
                {
                        return score > action.score;
                }

                int     x;
                int     y;
                float   score;
        };

        float           minimizer(State& s, float alpha, float beta,
                                  unsigned depth, const unsigned& limit,
                                  std::vector<Move>& path, StopWatch& watch) const;
        float           maximizer(State& s, float alpha, float beta,
                                  unsigned depth, const unsigned& limit,
                                  std::vector<Move>& path, StopWatch& watch) const;
        float           abmin_max_move(State& s, unsigned limit,
                                       std::vector<Move>& path, StopWatch& watch,
                                       float* score_map) const;
        void            build_actions_fast(State& s, unsigned depth, unsigned limit, std::vector<AvailableAction>& actions) const;

        HeuristicCostBenefit* m_heur;
};


#endif  // STRATEGYDFS
