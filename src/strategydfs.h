#ifndef STRATEGYDFS
#define STRATEGYDFS

#include <vector>
#include "gamenode.h"
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
        float           minimizer(State& s, float alpha, float beta,
                                  unsigned depth, const unsigned& limit, const std::vector<Move>& suggestions,
                                  std::vector<Move>& path, StopWatch& watch) const;
        float           maximizer(State& s, float alpha, float beta,
                                  unsigned depth, const unsigned& limit, const std::vector<Move>& suggestions,
                                  std::vector<Move>& path, StopWatch& watch) const;
        float           abmin_max_move(State& s, unsigned limit, const std::vector<Move>& suggestions,
                                       std::vector<Move>& path, StopWatch& watch,
                                       float* score_map) const;
        void            build_all_actions(State& s, std::vector<HeuristicGameNode>& actions) const;
        void            build_actions_fast(State& s, unsigned depth, unsigned limit, const std::vector<Move>& suggestions, std::vector<HeuristicGameNode>& actions) const;
        void            build_first_level(State& s, float* score_map, std::vector<HeuristicGameNode>& actions) const;

        IHeuristic*     m_heur;
};


#endif  // STRATEGYDFS
