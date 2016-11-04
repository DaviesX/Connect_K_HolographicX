#include "move.h"
#include "state.h"
#include "heurcostbenefit.h"

HeuristicCostBenefit::HeuristicCostBenefit()
{
}

struct LinkStat
{
        LinkStat(int who):
                who(who)
        {
                oppo = opponent_of(who);
        }

        void reset()
        {
                ins = 0;
                del = 0;
                state = 0;
        }

        int             who;
        unsigned        ins = 0;
        unsigned        state = 0;
        unsigned        del = 0;
        int             oppo;
};

static bool linkage(const char* val, int x, int y, unsigned dist, void* data)
{
        if (dist == 0)
                return true;
        LinkStat* ls = static_cast<LinkStat*>(data);
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

float HeuristicCostBenefit::benefit(const State& s, const Move& next_move, int who, int extra_moves) const
{
        // const float del_cost = 1.0f/std::max(0, ((int) s.k - 2));

        LinkStat ls(who), ls2(who);

        float score = 0.0f;
        for (unsigned d = 0; d < 4; d ++) {
                int x = next_move.x;
                int y = next_move.y;
                s.move_xy(x, y, d, &::linkage, &ls);
                if (ls.ins != 0) {
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
                s.move_xy(x, y, (d + 4)%8, &::linkage, &ls2);
                if (ls2.ins != 0) {
                        // Where there is a match.
                        if ((s.collides_edges(x, y, (d + 4)%8) ||
                             s.is(x, y) == opponent_of(who)))
                                // Where the link is blocked.
                                ls2.ins --;
                } else {
                        ls2.del = 0;
                }

                int exp = extra_moves + (int) (ls.ins + ls2.ins) - (int) (ls.del + ls2.del);
                if (exp >= 0)
                        score += exp*(1 << exp);

                ls.reset();
                ls2.reset();
        }
        return score;
}

float HeuristicCostBenefit::cost(const State& s, const Move& next_move, int who) const
{
        return benefit(s, next_move, who, 1);
}

void HeuristicCostBenefit::try_move(const State& s, const Move& m)
{
        m_stack.push_back(m_path_score);
        m_path_score += evaluate(s, m);
}

void HeuristicCostBenefit::untry_move()
{
        m_path_score = m_stack.back();
        m_stack.pop_back();
}

float HeuristicCostBenefit::evaluate(const State& s, const Move& next_move) const
{
        float cost = this->cost(s, next_move, opponent_of(State::AI_PIECE));
        float benefit = this->benefit(s, next_move, State::AI_PIECE, 1);
        float score = cost + benefit;
        return score + m_path_score;
}
