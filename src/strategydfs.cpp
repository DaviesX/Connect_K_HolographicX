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

float StrategyDFS::minimizer(State& s, float alpha, float beta, unsigned depth, const unsigned& limit) const
{
        std::vector<AvailableAction> actions;
        build_actions_fast(s, depth, actions);

        float score = beta;
        //float score = FLT_MAX;
        for (AvailableAction action: actions) {
                Move cur_move(action.x, action.y);
                s.set_move(action.x, action.y, State::HUMAN_PIECE);
                m_heur->try_move(s, cur_move);
                float cur_score;
                {
                        if (s.is_goal_for(cur_move, State::HUMAN_PIECE))
                                cur_score = -INFINITY;
                        else if (depth + 1 >= limit)
                                cur_score = m_heur->evaluate(s, cur_move);
                        else
                                cur_score = maximizer(s, alpha, score, depth + 1, limit);
                }
                m_heur->untry_move();
                s.set_move(action.x, action.y, State::NO_PIECE);

                if (cur_score <= alpha)
                        return -INFINITY;

                if (cur_score < score)
                        score = cur_score;
        }

        return score;
}

float StrategyDFS::maximizer(State& s, float alpha, float beta, unsigned depth, const unsigned& limit) const
{
        std::vector<AvailableAction> actions;
        build_actions_fast(s, depth, actions);

        float score = alpha;
        // float score = -FLT_MAX;
        for (AvailableAction action: actions) {
                Move cur_move(action.x, action.y);
                s.set_move(action.x, action.y, State::AI_PIECE);
                m_heur->try_move(s, cur_move);
                float cur_score;
                {
                        if (s.is_goal_for(cur_move, State::AI_PIECE))
                                cur_score = INFINITY;
                        else if (depth + 1 >= limit)
                                cur_score = m_heur->evaluate(s, cur_move);
                        else
                                cur_score = minimizer(s, score, beta, depth + 1, limit);
                }
                m_heur->untry_move();
                s.set_move(action.x, action.y, State::NO_PIECE);

                if (cur_score >= beta)
                        return INFINITY;

                if (cur_score > score)
                        score = cur_score;
        }

        return score;
}

void StrategyDFS::build_actions_fast(State& s, unsigned depth, std::vector<AvailableAction>& actions) const
{
        if (depth <= 4) {
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

const float CHECK_POINT_LIMIT = 0;
const float TIME_OUT_CODE = -533431;

float StrategyDFS::abmin_max_move(State& s, unsigned limit, Move& move, StopWatch& watch) const
{
        if (watch.check_point() <= CHECK_POINT_LIMIT)
                return TIME_OUT_CODE;

        bool has_set = false;
        float score = -INFINITY;

        std::vector<AvailableAction> actions;
        build_actions_fast(s, 0, actions);

        m_heur->load_state(s);

        for (AvailableAction action: actions) {
                s.set_move(action.x, action.y, State::AI_PIECE);
                m_heur->try_move(s, Move(action.x, action.y));
                float cur_score;
                if (s.is_goal_for(Move(action.x, action.y), State::AI_PIECE))
                        cur_score = INFINITY;
                else
                        cur_score = minimizer(s, -FLT_MAX, FLT_MAX, 1, limit);
                m_heur->untry_move();
                s.set_move(action.x, action.y, State::NO_PIECE);

                if (cur_score > score || (!has_set && cur_score >= score)) {
                        move.set(action.x, action.y);
                        score = cur_score;
                        has_set = true;
                }

                if (watch.check_point() <= CHECK_POINT_LIMIT)
                        return TIME_OUT_CODE;
        }
        return score;
}

void StrategyDFS::make_move(const State& s, Move& m) const
{
        //std::srand(std::time(nullptr));
        StopWatch watch;
        watch.begin(10000);

        Move curr;
        unsigned d = 1;
        do {
                float score = abmin_max_move(const_cast<State&>(s), d ++, curr, watch);
                if (score == TIME_OUT_CODE)
                        break;
                std::cout << "Accomplished depth " << d - 1 << ", selecting " << curr << std::endl;
                m = curr;
        } while (1);
}


void StrategyDFS::print_analysis(std::ostream& os, const State& k, int depth) const
{
        State& s = (State&) k;

        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        if (s.is(x, y) != State::NO_PIECE) {
                                os << "FORBID" << "\t";
                                continue;
                        }

                        s.set_move(x, y, State::AI_PIECE);
                        m_heur->try_move(s, Move(x, y));
                        float cur_score = minimizer(s, -FLT_MAX, FLT_MAX, 1, depth);
                        m_heur->untry_move();
                        s.set_move(x, y, State::NO_PIECE);

                        os << cur_score << "\t";
                }
                os << std::endl;
        }
}

void StrategyDFS::print_analysis(std::ostream& os, const State& k, int depth, unsigned x, unsigned y) const
{
        State& s = (State&) k;

        if (s.is(x, y) != State::NO_PIECE) {
                os << "FORBID" << "\t";
                return;
        }

        s.set_move(x, y, State::AI_PIECE);
        m_heur->try_move(s, Move(x, y));
        float cur_score = minimizer(s, -FLT_MAX, FLT_MAX, 1, depth);
        m_heur->untry_move();
        s.set_move(x, y, State::NO_PIECE);

        os << "score: " << cur_score << "\t";
        os << std::endl;
}
