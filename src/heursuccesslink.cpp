#include <string>
#include <vector>
#include <utility>
#include <state.h>
#include <heursuccesslink.h>

typedef std::pair<unsigned, unsigned> chess_pos_t;


static bool eval_fr(const int* val, int x, int y, unsigned dist, void* data)
{
        return *val == State::NO_PIECE;
}

// Tells me the effective degrees of freedom in direction d.
static float fr(const State& s, int x, int y, int who, unsigned d, unsigned l, unsigned k)
{
        float fr = s.scan(x, y, d, ::eval_fr, nullptr);
        return k/(k - l)*fr + k;
}

static bool eval_s(const int* val, int x, int y, unsigned dist, void* data)
{
        return *val == *(int*) data;
}

// Tells me the length of the connect in direction d.
static float s(const State& s, int x, int y, int who, unsigned d)
{
        return s.scan(x, y, d, ::eval_s, &who);
}

static float eval_xy(const State& s, int x, int y, int who)
{
        float score = 0;
        for (unsigned d = 0; d < 8; d ++) {
                float l = ::s(s, x, y, who, d);
                score += ::fr(s, x, y, who, (d + 4)%8, l, s.k)*l;
        }
        return score;
}

struct EvalAffectedData
{
        EvalAffectedData(int who, std::vector<chess_pos_t> chesses):
                who(who),
                affected(chesses)
        {
        }

        int                             who;
        std::vector<chess_pos_t>&       affected;
};

static bool eval_affected(const int* val, int x, int y, unsigned dist, void* data)
{
        EvalAffectedData* af_data = (EvalAffectedData*) &data;
        if (*val == State::NO_PIECE) {
                return true;
        } else if (*val == af_data->who) {
                af_data->affected.push_back(chess_pos_t(x, y));
                return false;
        } else {
                return false;
        }
}

static void find_affected_chess(const State& s, int who, const Move& move, std::vector<chess_pos_t>& chesses)
{
        EvalAffectedData data(who, chesses);
        for (unsigned d = 0; d < 8; d ++) {
                s.scan(move.col, move.row, d, ::eval_affected, &data);
        }
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

// Public API.
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

