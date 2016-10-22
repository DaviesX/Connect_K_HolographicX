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

float StrategyDFS::minimizer(State& s, Move& best_move, unsigned depth, unsigned limit)
{

        if (s.is_goal() || depth >= limit) {
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
                        float cur_score = maximizer(s, best_move, depth + 1, limit);
                        s.pop_move();

                        if (cur_score < score) {
                                //best_move.set(x, y);
                                score = cur_score;
                        }
                }
        }
        return score;
}

float StrategyDFS::maximizer(State& s, Move& best_move, unsigned depth, unsigned limit)
{
        if (s.is_goal() || depth >= limit) {
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
                        float cur_score = minimizer(s, best_move, depth + 1, limit);
                        s.pop_move();

                        //std::cout << cur_score << "\t";

                        if (cur_score > score) {
                                best_move.set(x, y);
                                score = cur_score;
                        }
                }
                //std::cout << std::endl;
        }

        return score;
}

void StrategyDFS::make_move(const State& s, Move& m)
{
        float score = maximizer((State&) s, m, 0, 4);
        std::cout << "score:" << score << std::endl;
}

