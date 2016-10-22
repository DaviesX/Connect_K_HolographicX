#include <math.h>
#include <ostream>
#include <log.h>
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

void StrategyDFS::make_move(const State& s, Move& m)
{
        //::get_log_stream() << m << std::endl;
        //::flush();

        float score = -INFINITY;
        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        if (s.is(x, y) != State::NO_PIECE)
                                continue;

                        float cur_score = m_heur->evaluate(s, Move(x, y), State::AI_PIECE);
                        if (cur_score > score) {
                                m.set(x, y);
                                score = cur_score;
                        }
                }
        }
        ::get_log_stream() << m << std::endl;
        ::flush();
}

