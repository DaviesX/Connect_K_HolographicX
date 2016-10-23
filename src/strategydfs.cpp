#include <math.h>
#include <iostream>
#include <ostream>
#include <state.h>
#include <move.h>
#include <cmath>
#include <strategydfs.h>
#include <iheuristic.h>
#include <heursuccesslink.h>


StrategyDFS::StrategyDFS()
{
        m_heur = new HeuristicSuccessLink();
}

StrategyDFS::~StrategyDFS()
{
        delete m_heur;
}

void StrategyDFS::load_state(const State& s)
{
        m_heur->load_state(s);
}

float StrategyDFS::minimizer(State& s, unsigned depth, unsigned limit)
{
        if (s.is_goal())
                return INFINITY;

        if (depth >= limit) {
                Move last;
                s.prev_move(last);
                s.set_move(last.col, last.row, State::NO_PIECE);
                float score = m_heur->evaluate(s, last, State::AI_PIECE);
                s.set_move(last.col, last.row, State::AI_PIECE);
                return score;
        }

        float score = +INFINITY;
        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        if (s.is(x, y) != State::NO_PIECE)
                                continue;

                        s.push_move(x, y, State::HUMAN_PIECE);
                        float cur_score = maximizer(s, depth + 1, limit);
                        s.pop_move();

                        if (cur_score < score)
                                score = cur_score;
                }
        }
        return score;
}

float StrategyDFS::maximizer(State& s, unsigned depth, unsigned limit)
{
        if (s.is_goal())
                return -INFINITY;

        if (depth >= limit) {
                Move last;
                s.prev_move(last);
                s.set_move(last.col, last.row, State::NO_PIECE);
                float score = m_heur->evaluate(s, last, State::HUMAN_PIECE);
                s.set_move(last.col, last.row, State::HUMAN_PIECE);
                return score;
        }

        float score = -INFINITY;
        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        if (s.is(x, y) != State::NO_PIECE)
                                continue;

                        s.push_move(x, y, State::AI_PIECE);
                        float cur_score = minimizer(s, depth + 1, limit);
                        s.pop_move();

                        if (cur_score >= score)
                                score = cur_score;
                }
        }

        return score;
}

float StrategyDFS::min_max_move(State& s, unsigned limit, Move& move)
{
        bool has_set = false;

        float score = -INFINITY;
        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        if (s.is(x, y) != State::NO_PIECE)
                                continue;

                        s.push_move(x, y, State::AI_PIECE);
                        float cur_score = minimizer(s, 1, limit);
                        s.pop_move();

                        if (cur_score > score || (!has_set && cur_score >= score)) {
                                move.set(x, y);
                                score = cur_score;
                                has_set = true;
                        }
                }
        }
        return score;
}

void StrategyDFS::make_move(const State& s, Move& m)
{
        min_max_move((State&) s, 4, m);
}


void StrategyDFS::print_analysis(std::ostream& os, const State& k, int depth)
{
        State& s = (State&) k;

        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        if (s.is(x, y) != State::NO_PIECE) {
                                os << "FORBID" << "\t";
                                continue;
                        }

                        s.push_move(x, y, State::AI_PIECE);
                        float cur_score = minimizer(s, 1, depth);
                        s.pop_move();

                        os << cur_score << "\t";
                }
                os << std::endl;
        }
}
