#include <float.h>
#include <cmath>
#include "move.h"
#include "state.h"
#include "heurcostbenefit.h"


HeuristicCostBenefit::HeuristicCostBenefit(bool fast_eval):
        m_fast_eval(fast_eval)
{
}

HeuristicCostBenefit::~HeuristicCostBenefit()
{
        delete [] m_exp_map;
}

static bool linkage(const char* val, int x, int y, unsigned dist, void* data)
{
        HeuristicCostBenefit::LinkStat* ls = static_cast<HeuristicCostBenefit::LinkStat*>(data);
        if (*val == ls->who) {
                ls->ins ++;
                ls->state = 1;
                return true;
        } else if (*val == State::NO_PIECE) {
                if (ls->state != 0) {
                        return false;
                } else {
                        ls->del ++;
                        return true;
                }
        } else
                return false;
}

void HeuristicCostBenefit::exponent(const State& s, const Move& m, Exp& exp) const
{
}

void HeuristicCostBenefit::load_state(const State& s)
{
        if (m_exp_map == nullptr ||
            m_exp_w != s.num_cols ||
            m_exp_h != s.num_rows) {
                m_exp_w = s.num_cols;
                m_exp_h = s.num_rows;
                m_exp_map = new Exp [m_exp_w*m_exp_h];
        }
}

float HeuristicCostBenefit::benefit(const State& s, const Move& next_move,
                                    int who, int extra_moves) const
{
        LinkStat ls(who), ls2(who);

        float score = 0.0f;
        for (unsigned d = 0; d < 4; d ++) {
                int x = next_move.x;
                int y = next_move.y;
                unsigned dist = s.move_xy(x, y, d, &::linkage, &ls);
                if (ls.ins != 0 || dist == 1) {
                        // Where there is a match.
                        if ((s.collides_edges(x, y, d) ||
                             s.is(x, y) == opponent_of(who)))
                                // Where the link is blocked.
                                ls.ins --;
                } else {
                        ls.del = 0;
                }

                x = next_move.x;
                y = next_move.y;
                dist = s.move_xy(x, y, (d + 4)%8, &::linkage, &ls2);
                if (ls2.ins != 0 || dist == 1) {
                        // Where there is a match.
                        if ((s.collides_edges(x, y, (d + 4)%8) ||
                             s.is(x, y) == opponent_of(who)))
                                // Where the link is blocked.
                                ls2.ins --;
                } else {
                        ls2.del = 0;
                }

                int exp = extra_moves + (int) (ls.ins + ls2.ins) - /*3.0f/4.0f**/((float) (ls.del + ls2.del));
                if (exp >= 0)
                        score += exp*exp*(1 << exp);

                ls.reset();
                ls2.reset();
        }
        return score;
}

void HeuristicCostBenefit::try_move(const State& s, const Move& m)
{
        m_stack.push_back(m);
}

void HeuristicCostBenefit::untry_move()
{
        m_stack.pop_back();
}

float HeuristicCostBenefit::evaluate_move(const State& s, const Move& move, int who) const
{
        if (who == State::HUMAN_PIECE)
                return benefit(s, move, who, 2);
        else
                return benefit(s, move, who, 1);
}

float HeuristicCostBenefit::coarse_eval(const State& s, const Move& next_move) const
{
        return evaluate(s, next_move);
}

float HeuristicCostBenefit::evaluate(const State& s, const Move& next_move) const
{
        float s0 = 0;
        float s1 = 0;

        float n0 = 1;
        float n1 = 1;

        std::vector<Move>& sequence = const_cast<std::vector<Move>&>(m_stack);
        sequence.push_back(next_move);
        for (unsigned i = 0; i < sequence.size(); i ++) {
                const Move& m = sequence[i];
                if ((i & 1) == 0) {
                        s0 += evaluate_move(s, m, State::AI_PIECE);
                        n0 ++;
                } else {
                        s1 += evaluate_move(s, m, State::HUMAN_PIECE);
                        n1 ++;
                }
        }
        sequence.pop_back();
        return s0/n0 - s1/n1;
}
