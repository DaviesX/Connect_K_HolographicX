#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <state.h>
#include <heursuccesslink.h>

typedef std::pair<unsigned, unsigned> chess_pos_t;


static bool eval_fr(const int* val, int x, int y, unsigned dist, void* data)
{
        return (*val == State::NO_PIECE && dist <= *(unsigned*) data) || (dist == 0);
}

// Tells me the effective degrees of freedom in direction d.
static float fr(const State& s, int x, int y, int who, unsigned d, int l, int k)
{
        unsigned r = std::max(0, k - l);
        return s.scan(x, y, d, ::eval_fr, &r) + k/(float) r;
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
                float alpha = ::fr(s, x, y, who, (d + 4)%8, l, s.k);
                score += alpha*l;
        }
        return score;
}

struct EvalAffectedData
{
        EvalAffectedData(std::vector<chess_pos_t>& ai_chesses,
                         std::vector<chess_pos_t>& oppo_chesses):
                ai_chesses(ai_chesses),
                oppo_chesses(oppo_chesses)
        {
        }

        std::vector<chess_pos_t>&       ai_chesses;
        std::vector<chess_pos_t>&       oppo_chesses;
};

static bool eval_affected(const int* val, int x, int y, unsigned dist, void* data)
{
        EvalAffectedData* af_data = (EvalAffectedData*) data;

        switch (*val) {
                case State::NO_PIECE:
                        return true;
                case State::AI_PIECE:
                        af_data->ai_chesses.push_back(chess_pos_t(x, y));
                        return false;
                case State::HUMAN_PIECE:
                        af_data->oppo_chesses.push_back(chess_pos_t(x, y));
                        return false;
        }
        return true;
}

static void find_affected_chess(const State& s, const Move& move,
                                std::vector<chess_pos_t>& ai_chesses, std::vector<chess_pos_t>& oppo_chesses)
{
        EvalAffectedData data(ai_chesses, oppo_chesses);
        for (unsigned d = 0; d < 8; d ++) {
                s.scan(move.col, move.row, d, ::eval_affected, &data);
        }
}

static float full_board_eval_for(const State& s, int who)
{
        float score = 0;
        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        score += ::eval_xy(s, x, y, who);
                }
        }
        return score;
}

static float full_board_eval(const State& k, const Move& next_move, int who)
{
        // Faking a const operation.
        State& s = (State&) k;

        float old_ai_score = full_board_eval_for(s, State::AI_PIECE);
        float old_oppo_score = full_board_eval_for(s, State::HUMAN_PIECE);

        s.set_move(next_move.col, next_move.row, who);
        float new_ai_score = full_board_eval_for(s, State::AI_PIECE);
        float new_oppo_score = full_board_eval_for(s, State::HUMAN_PIECE);
        s.set_move(next_move.col, next_move.row, State::NO_PIECE);

        float p0 = - old_ai_score + new_ai_score;
        float p1 = - old_oppo_score + new_oppo_score;

        return p0 - p1;
}

static float incremental_eval(const State& k, const Move& next_move, const int who)
{
        // Faking a const operation.
        State& s = (State&) k;

        std::vector<chess_pos_t> affected_ai, affected_oppo;
        ::find_affected_chess(s, next_move, affected_ai, affected_oppo);
/*
        float old_ai_score = 0;
        for (unsigned i = 0; i < affected_ai.size(); i ++) {
                old_ai_score += ::eval_xy(s, affected_ai[i].first, affected_ai[i].second, State::AI_PIECE);
        }
        float old_oppo_score = 0;
        for (unsigned i = 0; i < affected_oppo.size(); i ++) {
                old_oppo_score += ::eval_xy(s, affected_oppo[i].first, affected_oppo[i].second, State::HUMAN_PIECE);
        }
*/
        s.set_move(next_move.col, next_move.row, who);
        float new_ai_score = 0;
        for (unsigned i = 0; i < affected_ai.size(); i ++) {
                new_ai_score += ::eval_xy(s, affected_ai[i].first, affected_ai[i].second, State::AI_PIECE);
        }
        float new_oppo_score = 0;
        for (unsigned i = 0; i < affected_oppo.size(); i ++) {
                new_oppo_score += ::eval_xy(s, affected_oppo[i].first, affected_oppo[i].second, State::HUMAN_PIECE);
        }

        if (who == State::AI_PIECE)
                new_ai_score += ::eval_xy(s, next_move.col, next_move.row, State::AI_PIECE);
        else
                new_oppo_score += ::eval_xy(s, next_move.col, next_move.row, State::HUMAN_PIECE);
        s.set_move(next_move.col, next_move.row, State::NO_PIECE);

        float p0 = /*- old_ai_score +*/ new_ai_score;
        float p1 = /*- old_oppo_score +*/ new_oppo_score;

        return p0 - p1;
}

// Public API.
float HeuristicSuccessLink::evaluate(const State& k, const Move& next_move, int who)
{
        return ::incremental_eval(k, next_move, who);
        //return ::full_board_eval(k, next_move, who);
}
