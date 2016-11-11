#include <algorithm>
#include <iostream>
#include <ostream>
#include <float.h>
#include <cmath>
#include <ctime>
#include "stopwatch.h"
#include "state.h"
#include "move.h"
#include "strategydfs.h"
#include "iheuristic.h"
#include "heurchessdeg.h"
#include "heurcostbenefit.h"


#define Heuristics      HeuristicCostBenefit

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

StrategyDFS::StrategyDFS()
{
}

StrategyDFS::~StrategyDFS()
{
}

static float minimizer(State& s, Heuristics& h, float alpha, float beta,
                       unsigned depth, const unsigned& limit, std::vector<Move>& path, StopWatch& watch);
static float maximizer(State& s, Heuristics& h, float alpha, float beta,
                       unsigned depth, const unsigned& limit, std::vector<Move>& path, StopWatch& watch);
static float abmin_max_move(State& s, Heuristics& h, unsigned limit,
                            std::vector<Move>& path, StopWatch& watch, float* score_map);
static void build_actions_fast(State& s, const Heuristics& h, unsigned depth, unsigned limit,
                               std::vector<AvailableAction>& actions);

static void build_actions_fast(State& s, const Heuristics& h, unsigned depth, unsigned limit,
                               std::vector<AvailableAction>& actions)
{
        if (depth < limit - 1) {
                for (unsigned y = 0; y < s.num_rows; y ++) {
                        for (unsigned x = 0; x < s.num_cols; x ++) {
                                if (s.is(x, y) != State::NO_PIECE)
                                        continue;
                                float score = h.coarse_eval(s, Move(x, y));
                                actions.push_back(AvailableAction(x, y, score));
                        }
                }
                if ((depth & 1) == 0)
                        std::sort(actions.begin(), actions.end(),
                                  [](const AvailableAction& a, const AvailableAction& b) {return a > b;});
                else
                        std::sort(actions.begin(), actions.end(),
                                  [](const AvailableAction& a, const AvailableAction& b) {return a < b;});

        } else {
                for (unsigned y = 0; y < s.num_rows; y ++) {
                        for (unsigned x = 0; x < s.num_cols; x ++) {
                                if (s.is(x, y) != State::NO_PIECE)
                                        continue;
                                actions.push_back(AvailableAction(x, y, 0));
                        }
                }
                std::random_shuffle(actions.begin(), actions.end());
        }
}

static void print_path(std::ostream& os, std::vector<Move> path)
{
        os << "Path = [";
        for (unsigned i = 0; i < path.size(); i ++) {
                os << path[i];
                if (i != path.size() - 1)
                        os << ", ";
        }
        os << "]";
}

static void copy_path(std::vector<Move>& src, unsigned depth, unsigned limit, std::vector<Move>& dst)
{
        for (unsigned i = depth + 1; i < limit; i ++)
                dst[i] = src[i];
}

const float TIME_OUT_CODE = 134;
const float CHECK_POINT_LIMIT = 0;

static float minimizer(State& s, Heuristics& h, float alpha, float beta,
                       unsigned depth, const unsigned& limit, std::vector<Move>& path, StopWatch& watch)
{
        if (watch.check_point() <= CHECK_POINT_LIMIT)
                return TIME_OUT_CODE;

        std::vector<AvailableAction> actions;
        build_actions_fast(s, h, depth, limit, actions);

        float score = beta;
        //float score = FLT_MAX;
        for (AvailableAction action: actions) {
                Move cur_move(action.x, action.y);

                std::vector<Move> sub_path(limit);
                s.set_move(action.x, action.y, State::HUMAN_PIECE);
                float cur_score;
                {
                        if (h.is_goal_for(s, cur_move, State::HUMAN_PIECE))
                                cur_score = -INFINITY;
                        else if (depth + 1 >= limit)
                                cur_score = h.evaluate(s, cur_move);
                        else {
                                h.try_move(s, cur_move);
                                cur_score = maximizer(s, h, alpha, score, depth + 1, limit, sub_path, watch);
                                h.untry_move();
                        }
                }
                s.set_move(action.x, action.y, State::NO_PIECE);

                if (cur_score == TIME_OUT_CODE)
                        return TIME_OUT_CODE;

                if (cur_score <= alpha)
                       return -INFINITY;

                if (cur_score < score) {
                        score = cur_score;
                        ::copy_path(sub_path, depth, limit, path);
                        path[depth] = cur_move;
                }
        }

        return score;
}

static float maximizer(State& s, Heuristics& h, float alpha, float beta,
                       unsigned depth, const unsigned& limit, std::vector<Move>& path, StopWatch& watch)
{
        if (watch.check_point() <= CHECK_POINT_LIMIT)
                return TIME_OUT_CODE;

        std::vector<AvailableAction> actions;
        build_actions_fast(s, h, depth, limit, actions);

        float score = alpha;
        //float score = -FLT_MAX;
        for (AvailableAction action: actions) {
                Move cur_move(action.x, action.y);

                std::vector<Move> sub_path(limit);
                s.set_move(action.x, action.y, State::AI_PIECE);
                float cur_score;
                {
                        if (h.is_goal_for(s, cur_move, State::AI_PIECE))
                                cur_score = INFINITY;
                        else if (depth + 1 >= limit) {
                                cur_score = h.evaluate(s, cur_move);
                        } else {
                                h.try_move(s, cur_move);
                                cur_score = minimizer(s, h, score, beta, depth + 1, limit, sub_path, watch);
                                h.untry_move();
                        }
                }
                s.set_move(action.x, action.y, State::NO_PIECE);

                if (cur_score == TIME_OUT_CODE)
                        return TIME_OUT_CODE;

                if (cur_score >= beta)
                        return INFINITY;

                if (cur_score > score) {
                        score = cur_score;
                        ::copy_path(sub_path, depth, limit, path);
                        path[depth] = cur_move;
                }
        }

        return score;
}

static float abmin_max_move(State& s, Heuristics& h, unsigned limit,
                            std::vector<Move>& path, StopWatch& watch, float* score_map)
{
        if (watch.check_point() <= CHECK_POINT_LIMIT)
                return TIME_OUT_CODE;

        path.clear();
        path.resize(limit);
        std::vector<Move> sub_path(limit);

        bool has_set = false;
        float score = -INFINITY;

        std::vector<AvailableAction> actions;
        build_actions_fast(s, h, 0, limit, actions);

        h.load_state(s);

        for (AvailableAction action: actions) {
                Move cur_move(action.x, action.y);

                s.set_move(action.x, action.y, State::AI_PIECE);
                float cur_score;
                {
                        if (h.is_goal_for(s, cur_move, State::AI_PIECE))
                                cur_score = INFINITY;
                        else if (limit == 1)
                                cur_score = h.evaluate(s, cur_move);
                        else {
                                h.try_move(s, cur_move);
                                cur_score = minimizer(s, h, -FLT_MAX, FLT_MAX, 1, limit, sub_path, watch);
                                h.untry_move();
                        }
                }
                s.set_move(action.x, action.y, State::NO_PIECE);

                if (cur_score == TIME_OUT_CODE)
                        return TIME_OUT_CODE;

                if (score_map)
                        score_map[action.x + s.num_cols*action.y] = cur_score;

                if (cur_score > score || (!has_set && cur_score >= score)) {
                        score = cur_score;
                        ::copy_path(sub_path, 0, limit, path);
                        path[0] = cur_move;
                        has_set = true;
                }
        }

        return score;
}

void StrategyDFS::make_move(const State& s, unsigned quality, unsigned time, Move& m) const
{
        //std::srand(std::time(nullptr));
        std::vector<Move> path;
        StopWatch watch;
        watch.begin(time);

        unsigned d = quality;
        unsigned max = s.num_left;
        do {
                HeuristicCostBenefit h(true, d);
                float score = abmin_max_move(const_cast<State&>(s), h,
                                             d ++, path, watch, nullptr);
                if (score == TIME_OUT_CODE)
                        break;
                std::cout << "Accomplished depth " << d - 1 << ", selecting ";
                ::print_path(std::cout, path);
                std::cout << std::endl;
                std::cout << "Current score: " << score << std::endl;
                m = path[0];
        } while (d < max);
}

void StrategyDFS::print_analysis(std::ostream& os, const State& s, int depth) const
{
        std::vector<Move> path;
        float* score_map = new float [s.num_cols*s.num_rows];
        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        if (s.is(x, y) != State::NO_PIECE)
                                score_map[x + y*s.num_cols] = NAN;
                        else
                                score_map[x + y*s.num_cols] = -INFINITY;
                }
        }

        HeuristicCostBenefit h(true, depth);

        StopWatch watch;
        watch.begin(20000);
        abmin_max_move(const_cast<State&>(s), h, depth, path, watch, score_map);

        ::print_path(os, path);
        os << std::endl;

        // Print score.
        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        os << score_map[x + y*s.num_cols] << "\t";
                }
                os << std::endl;
        }
        delete [] score_map;
}

void StrategyDFS::print_analysis(std::ostream& os, const State& k, int depth, unsigned x, unsigned y) const
{
        State& s = const_cast<State&>(k);

        if (s.is(x, y) != State::NO_PIECE) {
                os << "FORBID" << "\t";
                return;
        }

        std::vector<Move> path(depth);
        StopWatch watch;
        watch.begin(20000);

        HeuristicCostBenefit h(true, depth);

        s.set_move(x, y, State::AI_PIECE);
        h.try_move(s, Move(x, y));
        float cur_score = minimizer(s, h, -FLT_MAX, FLT_MAX, 1, depth, path, watch);
        h.untry_move();
        s.set_move(x, y, State::NO_PIECE);

        path[0] = Move(x, y);

        ::print_path(os, path);
        os << std::endl;
        os << "score: " << cur_score << "\t";
        os << std::endl;
}
