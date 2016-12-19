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

        float qi(unsigned n_sims) const
        {
                if (m_sims == 0)
                        return 0.0f;
                else
                        return (float) m_wins/m_sims + sqrt(2.0f*log(n_sims)/(float) m_sims);
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
                unsigned        n_losses;
        };

        MCGameTree(const State& s, unsigned max_depth);
        ~MCGameTree();
        GameNode&                       switch_to_optimal_node(unsigned& depth);
        GameNode&                       get_best_node();
        void                            expand_node(GameNode& node, unsigned depth);
        void                            sample_at(const GameNode& node, unsigned depth,
                                                  unsigned sample_count, Sample& sample);
        void                            back_propagate(const Sample& samples, GameNode& nodes, unsigned depth);

        float                           win_rate() const;
private:
        State                           m_s;
        GameNode                        m_root;
        GameNode**                      m_path;
        unsigned                        m_path_length = 0;
        unsigned                        m_max_depth;
        std::vector<Move>               m_node_buf;
        std::vector<unsigned short>     m_order_buf;
};

std::ostream& operator<< (std::ostream& os, const MCGameTree& mcgt)
{
        os << "E{X} = " << mcgt.win_rate() << ", PL = " << mcgt.m_path_length;
        return os;
}

MCGameTree::MCGameTree(const State& s, unsigned max_depth):
        m_s(s), m_max_depth(max_depth)
{
        m_path = new GameNode* [max_depth];

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
                                float qi = node->m_chn[i].qi(node->m_sims);
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

GameNode& MCGameTree::get_best_node()
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

void MCGameTree::expand_node(GameNode& node, unsigned depth)
{
        if (node.m_num_chn == 0) {
                node.m_chn = ::expand_state(m_s, m_node_buf, true);
                node.m_num_chn = m_node_buf.size();
        }
}

void MCGameTree::sample_at(const GameNode& node, unsigned depth,
                           unsigned sample_count, Sample& sample)
{
        ::expand_state(m_s, m_node_buf, false);
        m_order_buf.resize(m_node_buf.size());
        for (unsigned short i = 0; i < m_node_buf.size(); i ++)
                m_order_buf[i] = i;

        int player = (depth & 1) == 1 ? State::AI_PIECE : State::HUMAN_PIECE;

        if (m_s.is_goal_for(Move(node.m_x, node.m_y), player)) {
                sample.n_sims = sample_count;
                if (player == State::AI_PIECE) {
                        sample.n_wins = sample_count;
                        sample.n_losses = 0;
                } else {
                        sample.n_losses = sample_count;
                        sample.n_wins = 0;
                }
                return ;
        }

        const unsigned MAX_LOOKAHEAD = std::min(m_node_buf.size(), 64UL);
        Move* moves = new Move [MAX_LOOKAHEAD];
        sample.n_wins = 0;
        sample.n_losses = 0;
        sample.n_sims = sample_count;

        for (unsigned i = 0; i < sample_count; i ++) {
                std::random_shuffle(m_order_buf.begin(), m_order_buf.end());

                unsigned j;
                for (j = 0; j < MAX_LOOKAHEAD; ) {
                        // Roll out.
                        int cur_player = (j & 1) == 0 ? player : opponent_of(player);
                        const Move& chosen = m_node_buf[m_order_buf[j]];
                        m_s.set_move(chosen.x, chosen.y, cur_player);
                        moves[j ++] = chosen;
                        if (m_s.is_goal_for(chosen, cur_player)) {
                                if (cur_player == State::AI_PIECE)
                                        sample.n_wins ++;
                                else
                                        sample.n_losses ++;
                                break;
                        }
                }

                // Clear previous playout.
                for (unsigned l = 0; l < j; l ++) {
                        m_s.set_move(moves[l].x, moves[l].y, State::NO_PIECE);
                }
        }

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
                        m_path[i]->m_wins += sample.n_losses;
                        m_path[i]->m_sims += sample.n_sims;
                }
        }
}

float MCGameTree::win_rate() const
{
        return m_path[0]->m_wins/(float) m_path[0]->m_sims;
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
                mcgt.sample_at(selected.m_chn[selected.m_i_chn], depth + 1, sample_count, sample);
                mcgt.back_propagate(sample, selected.m_chn[selected.m_i_chn], depth + 1);
                selected.m_i_chn ++;
        }
}

void StrategyMCTS::make_move(const State& s, unsigned quality, unsigned time, Move& m) const
{
        unsigned sample_count = quality*100;
        const unsigned SUB_CYCLES = 200;

        StopWatch watch;
        watch.begin(20000);

        MCGameTree mcgt(s, 8);
        while (watch.check_point() > 0) {
                for (unsigned i = 0; i < SUB_CYCLES; i ++) {
                        ::search(sample_count, mcgt);
                }
                const GameNode& best = mcgt.get_best_node();
                m.set(best.m_x, best.m_y);
                std::cout << mcgt << ", Current move " << m << std::endl;
        }
}

void StrategyMCTS::print(std::ostream& os) const
{
}
