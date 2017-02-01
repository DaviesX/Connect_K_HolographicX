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
#include "heurultimate.h"


StrategyDFS::StrategyDFS()
{
        //m_heur = new HeuristicChessDegree();
        //m_heur = new HeuristicCostBenefit();
        m_heur = new HeuristicUltimate();
}

StrategyDFS::~StrategyDFS()
{
        delete m_heur;
}

void StrategyDFS::load_state(const State& s)
{
        m_heur->load_state(s);
}

void StrategyDFS::build_all_actions(State& s, std::vector<HeuristicGameNode>& actions) const
{
        if (!s.gravity_on) {
                for (unsigned y = 0; y < s.num_rows; y ++) {
                        for (unsigned x = 0; x < s.num_cols; x ++) {
                                if (s.is(x, y) != State::NO_PIECE)
                                        continue;
                                actions.push_back(HeuristicGameNode(x, y, 0));
                        }
                }
        } else {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        for (unsigned y = 0; y < s.num_rows; y ++) {
                                if (s.is(x, y) == State::NO_PIECE) {
                                        actions.push_back(HeuristicGameNode(x, y, 0));
                                        break;
                                }
                        }
                }
        }
}

void StrategyDFS::build_actions_fast(State& s, unsigned depth, unsigned limit, const std::vector<Move>& suggestions, std::vector<HeuristicGameNode>& actions) const
{
        build_all_actions(s, actions);

        Move suggested(255, 255);
        if (suggestions.size() > depth)
                suggested = suggestions[depth];

        if (depth < limit - 1) {
                for (HeuristicGameNode action: actions) {
                        if (action.x == suggested.x && action.y == suggested.y)
                                action.score = FLT_MAX;
                        else
                                action.score = m_heur->coarse_eval(s, Move(action.x, action.y));
                }

                if ((depth & 1) == 0)
                        std::sort(actions.begin(), actions.end(),
                                  [](const HeuristicGameNode& a, const HeuristicGameNode& b) {return a > b;});
                else
                        std::sort(actions.begin(), actions.end(),
                                  [](const HeuristicGameNode& a, const HeuristicGameNode& b) {return a < b;});

        } else {
                std::random_shuffle(actions.begin(), actions.end());
        }
}

void StrategyDFS::build_first_level(State& s, float* score_map, std::vector<HeuristicGameNode>& actions) const
{
        build_all_actions(s, actions);
        for (HeuristicGameNode action: actions)
                action.score = score_map[action.x + action.y*s.num_cols];

        std::sort(actions.begin(), actions.end(),
                  [](const HeuristicGameNode& a, const HeuristicGameNode& b) {return a > b;});
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

const float CHECK_POINT_LIMIT = 0;
const float TIME_OUT_CODE = -533431;

float StrategyDFS::minimizer(State& s, float alpha, float beta,
                             unsigned depth, const unsigned& limit, const std::vector<Move>& suggestions,
                             std::vector<Move>& path, StopWatch& watch) const
{
        if (watch.check_point() <= CHECK_POINT_LIMIT)
                return TIME_OUT_CODE;

        std::vector<HeuristicGameNode> actions;
        build_actions_fast(s, depth, limit, suggestions, actions);

        float score = beta;
        //float score = FLT_MAX;
        for (unsigned i = 0; i < actions.size(); i ++) {
                const HeuristicGameNode& action = actions[i];

                Move cur_move(action.x, action.y);

                std::vector<Move> sub_path(limit);
                s.set_move(action.x, action.y, State::HUMAN_PIECE);
                float cur_score;
                {
                        if (s.is_goal_for(cur_move, State::HUMAN_PIECE))
                                cur_score = -INFINITY;
                        else if (depth + 1 >= limit)
                                cur_score = m_heur->evaluate(s, cur_move);
                        else {
                                m_heur->try_move(s, cur_move);
                                cur_score = maximizer(s, alpha, score, depth + 1, limit, suggestions, sub_path, watch);
                                m_heur->untry_move();
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

float StrategyDFS::maximizer(State& s, float alpha, float beta,
                             unsigned depth, const unsigned& limit, const std::vector<Move>& suggestions,
                             std::vector<Move>& path, StopWatch& watch) const
{
        if (watch.check_point() <= CHECK_POINT_LIMIT)
                return TIME_OUT_CODE;

        std::vector<HeuristicGameNode> actions;
        build_actions_fast(s, depth, limit, suggestions, actions);

        float score = alpha;
        //float score = -FLT_MAX;
        for (unsigned i = 0; i < actions.size(); i ++) {
                const HeuristicGameNode& action = actions[i];

                Move cur_move(action.x, action.y);

                std::vector<Move> sub_path(limit);
                s.set_move(action.x, action.y, State::AI_PIECE);
                float cur_score;
                {
                        if (s.is_goal_for(cur_move, State::AI_PIECE))
                                cur_score = INFINITY;
                        else if (depth + 1 >= limit) {
                                cur_score = m_heur->evaluate(s, cur_move);
                        } else {
                                m_heur->try_move(s, cur_move);
                                cur_score = minimizer(s, score, beta, depth + 1, limit, suggestions, sub_path, watch);
                                m_heur->untry_move();
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

float StrategyDFS::abmin_max_move(State& s, unsigned limit, const std::vector<Move>& suggestions,
                                  std::vector<Move>& path, StopWatch& watch, float* score_map) const
{
        if (watch.check_point() <= CHECK_POINT_LIMIT)
                return TIME_OUT_CODE;

        path.clear();
        path.resize(limit);
        std::vector<Move> sub_path(limit);

        bool has_set = false;
        float score = -INFINITY;

        std::vector<HeuristicGameNode> actions;
        build_first_level(s, score_map, actions);

        m_heur->load_state(s);

        for (unsigned i = 0; i < actions.size(); i ++) {
                const HeuristicGameNode& action = actions[i];

                Move cur_move(action.x, action.y);

                s.set_move(action.x, action.y, State::AI_PIECE);
                float cur_score;
                {
                        if (s.is_goal_for(cur_move, State::AI_PIECE))
                                cur_score = INFINITY;
                        else if (limit == 1)
                                cur_score = m_heur->evaluate(s, cur_move);
                        else {
                                m_heur->try_move(s, cur_move);
                                cur_score = minimizer(s, -FLT_MAX, FLT_MAX, 1, limit, suggestions, sub_path, watch);
                                m_heur->untry_move();
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
        if (s.last_move.x == 255 && s.last_move.y == 255) {
                // Hard code first move.
                m.set(s.num_cols/2, s.num_rows/2);
                return;
        }

        std::vector<Move> path;
        std::vector<Move> suggestions;
        StopWatch watch;
        watch.begin(time);

        unsigned d = quality;
        unsigned max = s.num_left;

        float* score_map = new float [s.num_cols*s.num_rows];
        for (unsigned j = 0; j < s.num_rows; j ++) {
                for (unsigned i = 0; i < s.num_cols; i ++) {
                        score_map[i + j*s.num_cols] = 0;
                }
        }
        do {
                float score = abmin_max_move(const_cast<State&>(s),
                                             d ++, suggestions, path, watch, score_map);
                if (score == TIME_OUT_CODE)
                        break;
                std::cout << "Accomplished depth " << d - 1 << ", selecting ";
                ::print_path(std::cout, path);
                std::cout << std::endl;
                std::cout << "Current score: " << score << std::endl;
                m = path[0];
                suggestions = path;
        } while (d < max);
        delete [] score_map;
}

void StrategyDFS::print_analysis(std::ostream& os, const State& s, int depth) const
{
        std::vector<Move> path;
        std::vector<Move> suggestions;
        float* score_map = new float [s.num_cols*s.num_rows];
        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        if (s.is(x, y) != State::NO_PIECE)
                                score_map[x + y*s.num_cols] = NAN;
                        else
                                score_map[x + y*s.num_cols] = -INFINITY;
                }
        }

        StopWatch watch;
        watch.begin(20000);
        abmin_max_move(const_cast<State&>(s), depth, path, suggestions, watch, score_map);

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
        std::vector<Move> suggestions;
        StopWatch watch;
        watch.begin(20000);

        s.set_move(x, y, State::AI_PIECE);
        m_heur->try_move(s, Move(x, y));
        float cur_score = minimizer(s, -FLT_MAX, FLT_MAX, 1, depth, suggestions, path, watch);
        m_heur->untry_move();
        s.set_move(x, y, State::NO_PIECE);

        path[0] = Move(x, y);

        ::print_path(os, path);
        os << std::endl;
        os << "score: " << cur_score << "\t";
        os << std::endl;
}
