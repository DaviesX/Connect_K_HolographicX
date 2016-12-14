#include <limits>
#include <algorithm>
#include <vector>
#include <iostream>
#include "state.h"
#include "stopwatch.h"
#include "strategymcts.h"


struct AvailableAction
{
        AvailableAction(int x, int y)
        {
                pos[0] = x;
                pos[1] = y;
        }

        void qi(int n_sims)
        {
                q = (float) i_wins/i_sims + sqrt(2*log(n_sims)/i_sims);
        }

        bool operator < (const AvailableAction& action) const
        {
                return q < action.q;
        }

        bool operator > (const AvailableAction& action) const
        {
                return q > action.q;
        }

        union {
                unsigned char   pos[2];
                unsigned char   x;
                unsigned char   y;
                unsigned short  key;
        };
        unsigned int    i_wins = 0;
        unsigned int    i_sims = 0;
        float           q;
};

StrategyMCTS::StrategyMCTS()
{
}

static void build_all_actions(const State& s, std::vector<AvailableAction>& actions)
{
        if (!s.gravity_on) {
                for (unsigned y = 0; y < s.num_rows; y ++) {
                        for (unsigned x = 0; x < s.num_cols; x ++) {
                                if (s.is(x, y) != State::NO_PIECE)
                                        continue;
                                actions.push_back(AvailableAction(x, y));
                        }
                }
        } else {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        for (unsigned y = 0; y < s.num_rows; y ++) {
                                if (s.is(x, y) == State::NO_PIECE) {
                                        actions.push_back(AvailableAction(x, y));
                                        break;
                                }
                        }
                }
        }
}

void StrategyMCTS::load_state(const State& s)
{
}

void simulate(const State& s, std::vector<AvailableAction>& action_buf, std::vector<unsigned short>& order_buf,
              const unsigned n_sims, const unsigned max_steps, unsigned& n_wins)
{
        ::build_all_actions(s, action_buf);
        order_buf.resize(action_buf.size());
        for (unsigned short i = 0; i < action_buf.size(); i ++)
                order_buf[i] = i;

        State& k = const_cast<State&>(s);
        Move moves[max_steps];
        n_wins = 0;

        for (unsigned i = 0; i < n_sims; i ++) {
                std::random_shuffle(order_buf.begin(), order_buf.end());

                unsigned j;
                for (j = 0; j < max_steps; j ++) {
                        // Roll out.
                        unsigned who = (j & 1) == 0 ? State::AI_PIECE : State::HUMAN_PIECE;
                        const AvailableAction& chosen = action_buf[order_buf[j]];
                        const Move& chosen_move = Move(chosen.x, chosen.y);
                        k.set_move(chosen.x, chosen.y, who);
                        if (k.is_goal_for(chosen_move, who)) {
                                if (who == State::AI_PIECE)
                                        n_wins ++;
                                break;
                        }
                        moves[j] = chosen_move;
                }

                // Clear previous playout.
                for (unsigned l = 0; l < j; l ++) {
                        k.set_move(moves[l].x, moves[l].y, State::NO_PIECE);
                }
        }
}

static AvailableAction* max_action(std::vector<AvailableAction>& actions)
{
        unsigned max_i = 0;
        for (unsigned i = 1; i < actions.size(); i ++) {
                if (actions[i].q > actions[max_i].q)
                        max_i = i;
        }
        return &actions[max_i];
}

static const AvailableAction& best_action(const std::vector<AvailableAction>& actions)
{
        unsigned max_i = 0;
        for (unsigned i = 1; i < actions.size(); i ++) {
                if (actions[i].i_sims > actions[max_i].i_sims)
                        max_i = i;
        }
        return actions[max_i];
}

void StrategyMCTS::make_move(const State& s, unsigned quality, unsigned time, Move& m) const
{
        const unsigned MAX_DEPTH = 8;
        const unsigned SIM_DEPTH = (s.k + 1)*2;
        const unsigned MAX_SIMS = 128;
        const unsigned SUB_CYCLE_SIZE = 65536;

        State& k = const_cast<State&>(s);
        StopWatch watch;
        watch.begin(s.deadline);

        std::vector<AvailableAction> action_buf;
        std::vector<unsigned short> order_buf;
        std::vector<AvailableAction> path_actions[MAX_DEPTH];
        AvailableAction* last_move[MAX_DEPTH] = {nullptr};

        // Insert and select root.
        path_actions[0].push_back(AvailableAction(-1, -1));
        unsigned depth = 0;
        unsigned deepest = 0;
        AvailableAction* selected = &path_actions[0][0];
        last_move[0] = selected;

        while (watch.check_point() > 0) {
                for (unsigned i = 0; i < SUB_CYCLE_SIZE; i ++) {
                        // Find a node that has the best Q(i) at each level.
                        // Proceed to the next level if the best move is the same as the last move.
                        for (unsigned d = 1; d <= depth; d ++) {
                                selected = ::max_action(path_actions[d]);
                                if (selected != last_move[d]) {
                                        // Needed to clear old branch and switch to new branch at this level.
                                        for (unsigned p = d; p <= depth; p ++) {
                                                k.set_move(last_move[p]->x, last_move[p]->y, State::NO_PIECE);
                                        }
                                        depth = d;
                                        last_move[d] = selected;
                                        goto L_CONFLICTED;
                                }
                        }
// L_NO_CONFLICT:
                        selected = ::max_action(path_actions[++ depth]);
                        last_move[depth] = selected;
L_CONFLICTED:

                        unsigned who = (depth & 1) == 0 ? State::AI_PIECE : State::HUMAN_PIECE;
                        if (selected->x != 0XFF)
                                k.set_move(selected->x, selected->y, who);
                        last_move[depth] = selected;
                        deepest = std::max(deepest, depth);

                        // Expand the selected node.
                        ::build_all_actions(s, path_actions[depth + 1]);

                        // Simulate over all the children.
                        unsigned n_child_sims = 0;
                        unsigned n_child_wins = 0;
                        for (AvailableAction action: path_actions[depth + 1]) {
                                k.set_move(action.x, action.y, who);
                                unsigned i_wins;
                                unsigned sim_steps = MAX_SIMS >> depth;

                                ::simulate(s, action_buf, order_buf, sim_steps, SIM_DEPTH, i_wins);
                                k.set_move(action.x, action.y, State::NO_PIECE);

                                action.i_sims += sim_steps;
                                action.i_wins += i_wins;

                                n_child_sims += sim_steps;
                                n_child_wins += i_wins;

                                action.qi(last_move[depth]->i_sims);
                        }

                        // Back propagate all the way to root.
                        for (unsigned i = 0; i <= depth; i ++) {
                                last_move[i]->i_sims += n_child_sims;
                                last_move[i]->i_wins += n_child_wins;
                        }
                        for (unsigned i = 1; i <= depth; i ++) {
                                last_move[i]->qi(last_move[i - 1]->i_sims);
                        }
                }
                // Select move.
                const AvailableAction& best = ::best_action(path_actions[1]);
                m.set(best.x, best.y);

                std::cout << "Deepest lookahead " << deepest << std::endl
                          << "Number of simulations " << last_move[0]->i_sims << std::endl
                          << "Win rate " << last_move[0]->i_wins/last_move[0]->i_sims << std::endl
                          << "Move selected " << m << std::endl;
        }

        // Reset the board.
        for (unsigned p = 0; p <= depth; p ++) {
                k.set_move(last_move[p]->x, last_move[p]->y, State::NO_PIECE);
        }
}

void StrategyMCTS::print(std::ostream& os) const
{
}
