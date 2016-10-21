#include <string>
#include <vector>
#include <utility>
#include <state.h>
#include <heursuccesslink.h>

typedef std::pair<unsigned, unsigned> chess_pos_t;


// Tells me the effective degrees of freedom in direction d.
static float fr(const State& s, int x, int y, int who, unsigned d, unsigned l, unsigned k)
{
        float fr;
        return k/(k - l)*fr + k;
}

// Tells me the length of the connect in direction d.
static float s(const State& s, int x, int y, int who, unsigned d)
{
}

static float eval_xy(const State& s, int x, int y, int who)
{
        float score = 0;
        for (unsigned d = 0; d < 8; d ++) {
                float l = ::s(s, x, y, who, (d + 4)%8);
                score += ::fr(s, x, y, who, d, l, s.k)*l;
        }
        return score;
}

static void find_affected_chess(const State& s, int who, const Move& move, std::vector<chess_pos_t> chesses)
{
}

static float full_board_eval(const State& s, int who)
{
        float score = 0;
        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        score += ::eval_xy(s, x, y, who);
                }
        }
        return score;
}

static float incremental_eval(const State& k, const Move& next_move, int who)
{
        // Faking a const operation.
        State& s = (State&) k;

        std::vector<chess_pos_t> affected;
        ::find_affected_chess(s, State::AI_PIECE, next_move, affected);
        float old_score = 0;
        for (unsigned i = 0; i < affected.size(); i ++) {
                old_score += ::eval_xy(s, affected[i].first, affected[i].second, who);
        }

        float new_score = 0;
        s.set_move(next_move.col, next_move.row, who);
        for (unsigned i = 0; i < affected.size(); i ++) {
                new_score += ::eval_xy(s, affected[i].first, affected[i].second, who);
        }
        new_score += ::eval_xy(s, next_move.col, next_move.row, State::AI_PIECE);
        s.set_move(next_move.col, next_move.row, State::NO_PIECE);

        return new_score - old_score;
}



void HeuristicSuccessLink::load_state(const State& s)
{
        p0 = ::full_board_eval(s, State::AI_PIECE);
        p1 = ::full_board_eval(s, State::HUMAN_PIECE);
}

void HeuristicSuccessLink::accept(const Move& m, int who, float score)
{
        switch (who) {
                case State::AI_PIECE:
                        p0 = score;
                        break;
                case State::HUMAN_PIECE:
                        p1 = score;
                        break;
        }
}


float HeuristicSuccessLink::evaluate(const State& s, const Move& next_move, int who)
{
        float increment = ::incremental_eval(s, next_move, who);
        if (who == State::AI_PIECE)
                return (p0 + increment)/p1;
        else
                return p0/(p1 + increment);
}

