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


StrategyDFS::StrategyDFS()
{
        //m_heur = new HeuristicChessDegree();
        m_heur = new HeuristicCostBenefit();
}

StrategyDFS::~StrategyDFS()
{
        delete m_heur;
}

void StrategyDFS::load_state(const State& s)
{
        m_heur->load_state(s);
}

void StrategyDFS::build_actions_fast(State& s, unsigned depth, unsigned limit, std::vector<AvailableAction>& actions) const
{
        if (depth <= limit - 1) {
                for (unsigned y = 0; y < s.num_rows; y ++) {
                        for (unsigned x = 0; x < s.num_cols; x ++) {
                                if (s.is(x, y) != State::NO_PIECE)
                                        continue;
                                float score = m_heur->evaluate(s, Move(x, y));
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

float StrategyDFS::minimizer(State& s, float alpha, float beta,
                             unsigned depth, const unsigned& limit, std::vector<Move>& path) const
{
        std::vector<AvailableAction> actions;
        build_actions_fast(s, depth, limit, actions);

        float score = beta;
        //float score = FLT_MAX;
        Move best_move(-1, -1);
        for (AvailableAction action: actions) {
                Move cur_move(action.x, action.y);
                s.set_move(action.x, action.y, State::HUMAN_PIECE);
                float cur_score;
                {
                        if (s.is_goal_for(cur_move, State::HUMAN_PIECE))
                                return -INFINITY;
                        else if (depth + 1 >= limit)
                                cur_score = m_heur->evaluate(s, cur_move);
                        else {
                                m_heur->try_move(s, cur_move);
                                cur_score = maximizer(s, alpha, score, depth + 1, limit, path);
                                m_heur->untry_move();
                        }
                }
                s.set_move(action.x, action.y, State::NO_PIECE);

                if (cur_score <= alpha)
                       return -INFINITY;

                if (cur_score < score) {
                        score = cur_score;
                        best_move = cur_move;
                }
        }

        path[depth] = best_move;
        return score;
}

float StrategyDFS::maximizer(State& s, float alpha, float beta,
                             unsigned depth, const unsigned& limit, std::vector<Move>& path) const
{
        std::vector<AvailableAction> actions;
        build_actions_fast(s, depth, limit, actions);

        float score = alpha;
        //float score = -FLT_MAX;
        Move best_move(-1, -1);
        for (AvailableAction action: actions) {
                Move cur_move(action.x, action.y);
                s.set_move(action.x, action.y, State::AI_PIECE);
                float cur_score;
                {
                        if (s.is_goal_for(cur_move, State::AI_PIECE))
                                return INFINITY;
                        else if (depth + 1 >= limit)
                                cur_score = m_heur->evaluate(s, cur_move);
                        else {
                                m_heur->try_move(s, cur_move);
                                cur_score = minimizer(s, score, beta, depth + 1, limit, path);
                                m_heur->untry_move();
                        }
                }
                s.set_move(action.x, action.y, State::NO_PIECE);

                if (cur_score >= beta)
                        return INFINITY;

                if (cur_score > score) {
                        score = cur_score;
                        best_move = cur_move;
                }
        }

        path[depth] = best_move;
        return score;
}

const float CHECK_POINT_LIMIT = 0;
const float TIME_OUT_CODE = -533431;

float StrategyDFS::abmin_max_move(State& s, unsigned limit,
                                  std::vector<Move>& path, StopWatch& watch, float* score_map) const
{
        if (watch.check_point() <= CHECK_POINT_LIMIT)
                return TIME_OUT_CODE;

        path.clear();
        path.resize(limit);

        bool has_set = false;
        float score = -INFINITY;

        std::vector<AvailableAction> actions;
        build_actions_fast(s, 0, limit, actions);

        m_heur->load_state(s);

        Move best_move(-1, -1);
        for (AvailableAction action: actions) {
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
                                cur_score = minimizer(s, -FLT_MAX, FLT_MAX, 1, limit, path);
                                m_heur->untry_move();
                        }
                }
                s.set_move(action.x, action.y, State::NO_PIECE);

                if (score_map)
                        score_map[action.x + s.num_cols*action.y] = cur_score;

                if (cur_score > score || (!has_set && cur_score >= score)) {
                        best_move = cur_move;
                        score = cur_score;
                        has_set = true;
                }

                if (watch.check_point() <= CHECK_POINT_LIMIT)
                        return TIME_OUT_CODE;
        }
        path[0] = best_move;
        return score;
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

void StrategyDFS::make_move(const State& s, unsigned quality, unsigned time, Move& m) const
{
        //std::srand(std::time(nullptr));
        std::vector<Move> path;
        StopWatch watch;
        watch.begin(time);

        unsigned d = quality;
        unsigned max = s.num_left;
        do {
                float score = abmin_max_move(const_cast<State&>(s),
                                             d ++, path, watch, nullptr);
                if (score == TIME_OUT_CODE)
                        break;
                std::cout << "Accomplished depth " << d - 1 << ", selecting ";
                ::print_path(std::cout, path);
                std::cout << std::endl;
                m = path[0];
        } while (0);
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

        StopWatch watch;
        watch.begin(20000);
        abmin_max_move(const_cast<State&>(s), depth, path, watch, score_map);

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
        State& s = (State&) k;

        if (s.is(x, y) != State::NO_PIECE) {
                os << "FORBID" << "\t";
                return;
        }

        std::vector<Move> path;

        s.set_move(x, y, State::AI_PIECE);
        m_heur->try_move(s, Move(x, y));
        float cur_score = minimizer(s, -FLT_MAX, FLT_MAX, 1, depth, path);
        m_heur->untry_move();
        s.set_move(x, y, State::NO_PIECE);

        ::print_path(os, path);
        os << std::endl;
        os << "score: " << cur_score << "\t";
        os << std::endl;
}
