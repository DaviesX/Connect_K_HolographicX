#include <math.h>
#include <ostream>
#include <state.h>
#include <move.h>
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

void StrategyDFS::make_move(const State& s, Move& m)
{
        m.set(0, 0);

        if (m_first_time) {
                m_heur->load_state(s);
                m_first_time = false;
        }

        float min = INFINITY;
        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        float score = m_heur->evaluate(s, Move(x, y));
                        if (s.is(x, y) == State::NO_PIECE) {
                                if (score < min) {
                                        m.set(x, y);
                                        min = score;
                                }
                        }
                }
        }
}

