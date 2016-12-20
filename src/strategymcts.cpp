#include <limits>
#include <algorithm>
#include <vector>
#include <iostream>
#include "state.h"
#include "stopwatch.h"
#include "strategymcts.h"


struct GameNode
{
        GameNode():
                m_x(0xFF), m_y(0xFF)
        {
        }

        GameNode(int x, int y):
                m_x(x), m_y(y)
        {
        }

        float qi(unsigned n_sims, unsigned wb, unsigned nb) const
        {
                const float b = 1.0f;
                float beta = (float) nb/(m_sims + nb + 4.0f*b*b*m_sims*nb);
                //float beta = 0.0f;
                return (1.0f - beta)*(float) m_wins/m_sims + beta*(float) wb/nb + sqrt(2.0f*log(n_sims)/(float) m_sims);
        }

        union {
                struct {
                        unsigned char   m_x;
                        unsigned char   m_y;
                };
                unsigned short  key;
        };
        unsigned char   m_num_chn = 0;
        unsigned char   m_i_chn = 0;
        GameNode*       m_chn = nullptr;
        float           m_wins = 0.0f;
        unsigned        m_sims = 0;
};

StrategyMCTS::StrategyMCTS()
{
}

static GameNode* expand_state(const State& s, std::vector<Move>& buf, bool to_nodes)
{
        buf.clear();

        if (!s.gravity_on) {
                for (unsigned y = 0; y < s.num_rows; y ++) {
                        for (unsigned x = 0; x < s.num_cols; x ++) {
                                if (s.is(x, y) != State::NO_PIECE)
                                        continue;
                                buf.push_back(Move(x, y));
                        }
                }
        } else {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        for (unsigned y = 0; y < s.num_rows; y ++) {
                                if (s.is(x, y) == State::NO_PIECE) {
                                        buf.push_back(Move(x, y));
                                        break;
                                }
                        }
                }
        }

        if (to_nodes && !buf.empty()) {
                GameNode* nodes = new GameNode [buf.size()];
                for (unsigned i = 0; i < buf.size(); i ++) {
                        nodes[i].m_x = buf[i].x;
                        nodes[i].m_y = buf[i].y;
                }
                return nodes;
        } else
                return nullptr;
}

void StrategyMCTS::load_state(const State& s)
{
}

class MCGameTree
{
        friend std::ostream& operator<< (std::ostream& os, const MCGameTree& mcgt);
public:
        struct Sample
        {
                unsigned        n_sims;
                unsigned        n_wins;
                //unsigned        n_losses;
        };

        MCGameTree(const State& s, unsigned max_depth);
        ~MCGameTree();
        GameNode&                       switch_to_optimal_node(unsigned& depth);
        GameNode&                       get_best_node() const;
        GameNode&                       get_optimal_node() const;
        void                            expand_node(GameNode& node, unsigned depth);
        void                            sample_at(const GameNode& node, unsigned depth,
                                                  unsigned sample_count, Sample& sample);
        void                            back_propagate(const Sample& samples, GameNode& nodes, unsigned depth);
        Sample&                         get_rave(unsigned x, unsigned y);

        float                           win_rate() const;
        unsigned                        pl() const;
private:
        State                           m_s;
        Sample*                         m_nw;
        GameNode                        m_root;
        GameNode**                      m_path;
        unsigned                        m_path_length = 0;
        unsigned                        m_max_depth;
        std::vector<Move>               m_node_buf;
        std::vector<unsigned short>     m_order_buf;
};

std::ostream& operator<< (std::ostream& os, const MCGameTree& mcgt)
{
        os << "T = " << mcgt.m_path[0]->m_sims << ", E{X} = " << mcgt.win_rate() << ", PL = " << mcgt.m_path_length << ", ";
        for (unsigned i = 1; i < mcgt.m_path_length; i ++)
                os << Move(mcgt.m_path[i]->m_x, mcgt.m_path[i]->m_y) << ", ";
        return os;
}

MCGameTree::MCGameTree(const State& s, unsigned max_depth):
        m_s(s), m_max_depth(max_depth)
{
        m_path = new GameNode* [max_depth];

        m_nw = new Sample [m_s.num_cols*m_s.num_rows];
        for (unsigned i = 0; i < m_s.num_cols*m_s.num_rows; i ++) {
                m_nw[i].n_sims = 0;
                m_nw[i].n_wins = 0;
        }

        m_node_buf.reserve(s.num_left);
        m_order_buf.reserve(s.num_left);
}

static void MCGameTree_free(GameNode* node)
{
        if (node->m_num_chn == 0)
                return ;
        for (unsigned i = 0; i < node->m_num_chn; i ++)
                MCGameTree_free(&node->m_chn[i]);
        delete [] node->m_chn;
}

MCGameTree::~MCGameTree()
{
        delete [] m_path;
        delete [] m_nw;
        MCGameTree_free(&m_root);
}

GameNode& MCGameTree::switch_to_optimal_node(unsigned& depth)
{
        GameNode* node = &m_root;
        depth = 0;

        // Reset all the moves.
        for (unsigned i = 1; i < m_path_length; i ++) {
                m_s.set_move(m_path[i]->m_x, m_path[i]->m_y, State::NO_PIECE);
        }

        // Switch to the optimal branch.
        m_path_length = 0;
        while (true) {
                if (node->key != 0xFFFF)
                        m_s.set_move(node->m_x, node->m_y, ((depth & 1)) == 1 ? State::AI_PIECE : State::HUMAN_PIECE);
                m_path[m_path_length ++] = node;
                if (depth >= m_max_depth - 1) {
                        if (node->m_i_chn == node->m_num_chn)
                                node->m_i_chn = 0;
                        return *node;
                } else if (node->m_i_chn < node->m_num_chn) {
                        return *node;
                } else if (node->m_num_chn == 0) {
                        return *node;
                } else {
                        unsigned i_opti = 0;
                        float opti_qi = 0;
                        for (unsigned i = 0; i < node->m_num_chn; i ++) {
                                Sample& rave = get_rave(node->m_chn[i].m_x, node->m_chn[i].m_y);
                                float qi = node->m_chn[i].qi(node->m_sims, rave.n_wins, rave.n_sims);
                                if (qi > opti_qi) {
                                        opti_qi = qi;
                                        i_opti = i;
                                }
                        }
                        node = &node->m_chn[i_opti];
                        depth ++;
                }
        }
}

GameNode& MCGameTree::get_best_node() const
{
        int i_best = -1;
        unsigned most = 0;
        for (unsigned i = 0; i < m_path[0]->m_num_chn; i ++) {
                if (most < m_path[0]->m_chn[i].m_sims) {
                        i_best = i;
                        most = m_path[0]->m_chn[i].m_sims;
                }
        }
        if (i_best == -1)
                throw std::string("No children from root");
        return m_path[0]->m_chn[i_best];
}

GameNode& MCGameTree::get_optimal_node() const
{
        return *m_path[1];
}

void MCGameTree::expand_node(GameNode& node, unsigned depth)
{
        if (node.m_sims > 0 && node.m_wins >= node.m_sims) {
                // No children can be made at a goal state.
                return;
        }
        if (node.m_num_chn == 0) {
                node.m_chn = ::expand_state(m_s, m_node_buf, true);
                node.m_num_chn = m_node_buf.size();
        }
}

void MCGameTree::sample_at(const GameNode& node, unsigned depth,
                           unsigned sample_count, Sample& sample)
{
        if (node.m_sims > 0 && node.m_wins >= node.m_sims) {
                sample.n_sims = sample_count;
                sample.n_wins = sample_count;
                return ;
        }

        int player = (depth & 1) == 1 ? State::AI_PIECE : State::HUMAN_PIECE;
        m_s.set_move(node.m_x, node.m_y, player);
        if (m_s.is_goal_for(Move(node.m_x, node.m_y), player)) {
                sample.n_sims = sample_count;
                sample.n_wins = sample_count;

                Sample& rave = get_rave(node.m_x, node.m_y);
                rave.n_sims += sample_count;
                rave.n_wins += sample_count;

                m_s.set_move(node.m_x, node.m_y, State::NO_PIECE);
                return ;
        }

        ::expand_state(m_s, m_node_buf, false);

        const unsigned MAX_LOOKAHEAD = m_node_buf.size();
        Move* moves = new Move [MAX_LOOKAHEAD];
        sample.n_wins = 0;
        sample.n_sims = sample_count;

        for (unsigned i = 0; i < sample_count; i ++) {
                std::random_shuffle(m_node_buf.begin(), m_node_buf.end());

                bool has_won = false;
                unsigned j;
                for (j = 0; j < MAX_LOOKAHEAD; ) {
                        // Roll out.
                        int cur_player = (j & 1) == 0 ? opponent_of(player) : player;
                        const Move& chosen = m_node_buf[j];
                        m_s.set_move(chosen.x, chosen.y, cur_player);
                        moves[j] = chosen;
                        if (m_s.is_goal_for(chosen, cur_player)) {
                                if (cur_player == player)
                                        has_won = true;
                                break;
                        }
                        j ++;
                }
                if (j == MAX_LOOKAHEAD) {
                        // Break the tie.
                        unsigned v = std::rand();
                        if ((v & 1) == 0)
                                has_won = true;
                        j --;
                }

                if (has_won) {
                        sample.n_wins ++;
                        for (unsigned l = 0; l <= j; l ++)
                                get_rave(moves[l].x, moves[l].y).n_wins;
                }

                // Update RAVE statistics and clear previous playout.
                for (unsigned l = 0; l <= j; l ++) {
                        get_rave(moves[l].x, moves[l].y).n_sims ++;
                        m_s.set_move(moves[l].x, moves[l].y, State::NO_PIECE);
                }
        }

        m_s.set_move(node.m_x, node.m_y, State::NO_PIECE);

        delete [] moves;
}

void MCGameTree::back_propagate(const Sample& sample, GameNode& node, unsigned depth)
{
        node.m_sims += sample.n_sims;
        node.m_wins += sample.n_wins;

        if (depth >= 1) {
                for (int i = (int) depth - 2; i >= 0; i -= 2) {
                        m_path[i]->m_wins += sample.n_wins;
                        m_path[i]->m_sims += sample.n_sims;
                }

                for (int i = (int) depth - 1; i >= 0; i -= 2) {
                        m_path[i]->m_sims += sample.n_sims;
                }
        }
}

float MCGameTree::win_rate() const
{
        return 1.0f - m_path[0]->m_wins/(float) m_path[0]->m_sims;
}

MCGameTree::Sample& MCGameTree::get_rave(unsigned x, unsigned y)
{
        return m_nw[x + y*m_s.num_cols];
}

unsigned MCGameTree::pl() const
{
        return m_path_length;
}

static const GameNode& best_action(const std::vector<GameNode>& actions)
{
        unsigned max_i = 0;
        for (unsigned i = 1; i < actions.size(); i ++) {
                if (actions[i].m_sims > actions[max_i].m_sims)
                        max_i = i;
        }
        return actions[max_i];
}

static void search(unsigned sample_count, MCGameTree& mcgt)
{
        unsigned depth;
        GameNode& selected = mcgt.switch_to_optimal_node(depth);
        mcgt.expand_node(selected, depth);

        MCGameTree::Sample sample;
        if (depth == 0) {
                while (selected.m_i_chn < selected.m_num_chn) {
                        mcgt.sample_at(selected.m_chn[selected.m_i_chn], depth + 1, sample_count, sample);
                        mcgt.back_propagate(sample, selected.m_chn[selected.m_i_chn], depth + 1);
                        selected.m_i_chn ++;
                }
        } else {
                if (selected.m_num_chn == 0) {
                        // Make more samples at this goal state.
                        mcgt.sample_at(selected, depth, sample_count, sample);
                        mcgt.back_propagate(sample, selected, depth);
                        return ;
                } else {
                        mcgt.sample_at(selected.m_chn[selected.m_i_chn], depth + 1, sample_count, sample);
                        mcgt.back_propagate(sample, selected.m_chn[selected.m_i_chn], depth + 1);
                        selected.m_i_chn ++;
                }
        }
}

void StrategyMCTS::make_move(const State& s, unsigned quality, unsigned time, Move& m) const
{
        unsigned sample_count = 500;
        const unsigned SUB_CYCLES = 500;
        const unsigned DEPTH_LIMIT = 8;

        MCGameTree mcgt(s, DEPTH_LIMIT);
        StopWatch watch;
        //watch.begin(60000);

        while (mcgt.pl() < DEPTH_LIMIT) {
                for (unsigned i = 0; i < SUB_CYCLES; i ++) {
                        ::search(sample_count, mcgt);
                }
                const GameNode& best = mcgt.get_optimal_node();
                m.set(best.m_x, best.m_y);
                std::cout << mcgt << ", Current " << m << std::endl;
        }
}

void StrategyMCTS::print(std::ostream& os) const
{
}
