#include <float.h>
#include <cmath>
#include "move.h"
#include "state.h"
#include "heurcostbenefit.h"


HeuristicCostBenefit::HeuristicCostBenefit(bool fast_eval, unsigned target_depth):
        m_depth(target_depth), m_fast_eval(fast_eval)
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

static bool prop_dir(const char* val, int x, int y, unsigned dist, void* data)
{
}

void HeuristicCostBenefit::propagate(const State& s, const Move& m)
{
        //s.move_xy(m.x, m.y, d, ::prop_dir, );
}

void HeuristicCostBenefit::unpropagate()
{
}

static unsigned char eval_stat_link(const HeuristicCostBenefit::LinkStat& ls)
{
        return std::max(0, (int) ls.ins - 3*(int) ls.del/4);
}

void HeuristicCostBenefit::load_state(const State& s)
{
        if (!m_fast_eval)
                return;

        if (m_exp_map == nullptr ||
            m_exp_w != s.num_cols ||
            m_exp_h != s.num_rows) {
                m_exp_w = s.num_cols;
                m_exp_h = s.num_rows;
                m_exp_map = new Exp [m_exp_w*m_exp_h];
        }
        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        const Move curr_move(x, y);
                        LinkStat ls(State::AI_PIECE);
                        for (unsigned d = 0; d < 4; d ++) {
                                acc_link_stat(s, curr_move, State::AI_PIECE, d, ls);
                                acc_link_stat(s, curr_move, State::AI_PIECE, (d + 4)%8, ls);
                                m_exp_map[x + y*s.num_cols].ai_exp[d] = ::eval_stat_link(ls);
                                ls.reset();
                        }

                        LinkStat ls2(State::HUMAN_PIECE);
                        for (unsigned d = 0; d < 4; d ++) {
                                acc_link_stat(s, curr_move, State::AI_PIECE, d, ls2);
                                acc_link_stat(s, curr_move, State::AI_PIECE, (d + 4)%8, ls2);
                                m_exp_map[x + y*s.num_cols].oppo_exp[d] = ::eval_stat_link(ls);
                                ls2.reset();
                        }
                }
        }
}

void HeuristicCostBenefit::acc_link_stat(const State& s, const Move& next_move,
                                         int who, unsigned d, LinkStat& ls) const
{
        int del = ls.del;
        ls.state = 0;

        int x = next_move.x;
        int y = next_move.y;
        unsigned dist = s.move_xy(x, y, d, &::linkage, &ls);
        if (ls.ins != 0 || dist == 1) {
                // Where there is a match.
                if ((s.collides_edges(x, y, d) ||
                     s.is(x, y) == opponent_of(who)))
                        // Where the link is blocked.
                        ls.del ++;
        } else
                ls.del = del;
}

float HeuristicCostBenefit::benefit(const State& s, const Move& next_move,
                                    int who, int extra_moves) const
{
        LinkStat ls(who);

        float score = 0.0f;
        for (unsigned d = 0; d < 4; d ++) {
                acc_link_stat(s, next_move, who, d, ls);
                acc_link_stat(s, next_move, who, (d + 4)%8, ls);

                // s = x^2*2^x
                int exp = extra_moves + ::eval_stat_link(ls);
                if (exp >= 0)
                        score += exp*exp*(1 << exp);
                ls.reset();
        }
        return score;
}

float HeuristicCostBenefit::benefit_fast(const State& s, const Move& next_move, int who, int extra_moves) const
{
        const Exp& e = m_exp_map[next_move.x + m_exp_w*next_move.y];

        const char* ls;
        if (who == State::AI_PIECE)
                ls = e.ai_exp;
        else
                ls = e.oppo_exp;

        float score = 0.0f;
        for (unsigned d = 0; d < 4; d ++) {
                // s = x^2*2^x
                int exp = extra_moves + ls[d];
                if (exp >= 0)
                        score += exp*exp*(1 << exp);
        }
        return score;
}

void HeuristicCostBenefit::try_move(const State& s, const Move& m)
{
        m_path_stack.push_back(m);

        if (m_fast_eval && m_path_stack.size() <= m_depth - 1)
                propagate(s, m);
}

void HeuristicCostBenefit::untry_move()
{
        if (m_fast_eval && m_path_stack.size() <= m_depth - 1)
                unpropagate();

        m_path_stack.pop_back();
}

float HeuristicCostBenefit::evaluate_move(const State& s, const Move& move, int who) const
{
        if (m_fast_eval) {
                if (who == State::HUMAN_PIECE)
                        return benefit_fast(s, move, who, 2);
                else
                        return benefit_fast(s, move, who, 1);
        } else {
                if (who == State::HUMAN_PIECE)
                        return benefit(s, move, who, 2);
                else
                        return benefit(s, move, who, 1);
        }
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

        std::vector<Move>& sequence = const_cast<std::vector<Move>&>(m_path_stack);
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

void HeuristicCostBenefit::print(std::ostream &os) const
{
        os << "CostBenefit = [exp_map: " << std::endl;
        for (unsigned y = 0; y < m_exp_h; y ++) {
                for (unsigned x = 0; x < m_exp_w; x ++) {
                        m_exp_map[x + y*m_exp_w].print(os);
                        os << "\t";
                }
                os << std::endl;
        }
        os << "]";
}
