#include <limits>
#include <algorithm>
#include <vector>
#include <map>
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

        float beta() const
        {
                const float b = 0.01f;
                return (float) m_rsims/(m_sims + m_rsims + 4.0f*b*b*(float) m_sims*(float) m_rsims);
        }

        float qi(unsigned n_sims) const
        {
                const float c = 0.25f;
                float b = beta();
                return (1.0f - b)*(float) m_wins/m_sims + b*(float) m_rwins/m_rsims +
                        sqrt(c*log(n_sims)/(float) m_sims);
                //return (float) m_wins/m_sims + sqrt(c*log(n_sims)/(float) m_sims);
        }

        // Position and key.
        union {
                struct {
                        unsigned char   m_x;
                        unsigned char   m_y;
                };
                unsigned short  key;
        };

        GameNode* find_child(unsigned short key) const
        {
                int l = 0, h = m_num_chn - 1;
                while (l <= h) {
                        unsigned m = l + (((unsigned) (h - l)) >> 1);
                        if (key == m_chn[m].key)
                                return &m_chn[m];
                        else if (key < m_chn[m].key)
                                h = m - 1;
                        else
                                l = m + 1;
                }
                return nullptr;
        }

        // Childrens are sorted in key order.
        unsigned char   m_num_chn = 0;
        unsigned char   m_i_chn = 0;
        GameNode*       m_chn = nullptr;

        // Statistics.
        unsigned        m_wins = 0;
        unsigned        m_sims = 0;
        unsigned        m_rwins = 0;
        unsigned        m_rsims =  0;
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
        };

        struct SmallSample
        {
                union {
                        struct {
                                unsigned short  n_sims;
                                unsigned short  n_wins;
                        };
                        unsigned        a = 0;
                };
        };

        MCGameTree(const State& s, unsigned max_depth);
        ~MCGameTree();
        GameNode&                       switch_to_optimal_node(unsigned& depth, unsigned step_size);
        GameNode&                       get_best_node() const;
        GameNode&                       get_optimal_node() const;
        void                            expand_node(GameNode& node, unsigned depth);
        const SmallSample**             sample_at(const GameNode& node, unsigned depth,
                                                  unsigned sample_count, Sample& sample);
        void                            back_propagate(const Sample& sample, const SmallSample** psamples,
                                                       GameNode& nodes, unsigned depth);

        SmallSample&                    get_rave(unsigned p, unsigned x, unsigned y);
        const SmallSample&              get_rave(const SmallSample** rave, unsigned p, unsigned x, unsigned y) const;
        void                            rave_effective_samples(const SmallSample** rave, unsigned p,
                                                               std::vector<Move>& eff_moves) const;
        void                            clear_rave();
        float                           win_rate() const;
        unsigned                        pl() const;
        unsigned                        max_depth() const;
        void                            print_score_map(std::ostream& os) const;
private:
        State                           m_s;
        GameNode                        m_root;
        GameNode**                      m_path;
        unsigned                        m_path_length = 0;
        unsigned                        m_max_depth;
        std::vector<Move>               m_node_buf;
        SmallSample*                    m_psamp_buf[2];
};

std::ostream& operator<< (std::ostream& os, const MCGameTree& mcgt)
{
        os << "T = " << mcgt.m_path[0]->m_sims << ", E{X} = " << mcgt.win_rate() << ", PL = " << mcgt.m_path_length << ", ";
        for (unsigned i = 1; i < mcgt.m_path_length; i ++)
                os << Move(mcgt.m_path[i]->m_x, mcgt.m_path[i]->m_y) << ", ";
        return os;
}

MCGameTree::MCGameTree(const State& s, unsigned max_depth):
        m_s(s), m_max_depth(std::min(s.num_left, max_depth))
{
        m_path = new GameNode* [m_max_depth + 1];

        m_psamp_buf[0] = new SmallSample [s.num_cols*s.num_rows];
        m_psamp_buf[1] = new SmallSample [s.num_cols*s.num_rows];

        m_node_buf.reserve(s.num_left);
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
        delete [] m_psamp_buf[0];
        delete [] m_psamp_buf[1];
        MCGameTree_free(&m_root);
}

void MCGameTree::print_score_map(std::ostream& os) const
{
        for (unsigned y = 0; y < m_s.num_rows; y ++) {
                for (unsigned x = 0; x < m_s.num_cols; x ++) {
                        const GameNode* n = m_root.find_child(Move(x, y).key);
                        if (n)
                                //os << n->qi(m_root.m_sims) << "\t";
                                os << (float) n->m_wins/n->m_sims << "\t";
                        else
                                os << "NAN" << "\t";
                }
                os << std::endl;
        }
}

GameNode& MCGameTree::switch_to_optimal_node(unsigned& depth, unsigned step_size)
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
                if (depth >= m_max_depth) {
                        if (node->m_i_chn == node->m_num_chn)
                                node->m_i_chn = 0;
                        return *node;
                } else if (node->m_i_chn < node->m_num_chn) {
                        return *node;
                } else if (node->m_num_chn == 0) {
                        return *node;
                } else {
                        depth ++;
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

const MCGameTree::SmallSample** MCGameTree::sample_at(const GameNode& node, unsigned depth,
                                                      unsigned sample_count, Sample& sample)
{
        clear_rave();

        if (node.m_sims > 0 && node.m_wins >= node.m_sims) {
                sample.n_sims = sample_count;
                sample.n_wins = sample_count;

                return const_cast<const SmallSample**>(m_psamp_buf);
        }

        int player = (depth & 1) == 1 ? State::AI_PIECE : State::HUMAN_PIECE;
        m_s.set_move(node.m_x, node.m_y, player);
        if (m_s.is_goal_for(Move(node.m_x, node.m_y), player)) {
                sample.n_sims = sample_count;
                sample.n_wins = sample_count;

                m_s.set_move(node.m_x, node.m_y, State::NO_PIECE);
                return const_cast<const SmallSample**>(m_psamp_buf);
        }

        ::expand_state(m_s, m_node_buf, false);

        const unsigned MAX_LOOKAHEAD = m_node_buf.size();
        if (MAX_LOOKAHEAD == 0) {
                // Nothing to sample.
                sample.n_sims = sample_count;
                sample.n_wins = sample_count;

                return const_cast<const SmallSample**>(m_psamp_buf);
        }

        Move* moves = new Move [MAX_LOOKAHEAD];
        sample.n_wins = 0;
        sample.n_sims = sample_count;

        for (unsigned i = 0; i < sample_count; i ++) {
                std::random_shuffle(m_node_buf.begin(), m_node_buf.end());

                bool has_won = false;
                unsigned j;
                for (j = 0; j < MAX_LOOKAHEAD; ) {
                        // Rollout.
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

                        // Update RAVE.
                        for (unsigned l = 1; l <= j; l += 2) {
                                get_rave(0, moves[l].x, moves[l].y).n_wins ++;
                        }
                }

                // Clear previous playout and update RAVE.
                for (unsigned l = 0; l <= j; l ++) {
                        unsigned p = (l + 1) & 1;
                        get_rave(p, moves[l].x, moves[l].y).n_sims ++;
                        m_s.set_move(moves[l].x, moves[l].y, State::NO_PIECE);
                }
        }

        m_s.set_move(node.m_x, node.m_y, State::NO_PIECE);

        delete [] moves;
        return const_cast<const SmallSample**>(m_psamp_buf);
}

void MCGameTree::back_propagate(const Sample& sample, const SmallSample** psamples,
                                GameNode& node, unsigned depth)
{
        node.m_sims += sample.n_sims;
        node.m_wins += sample.n_wins;

        if (depth >= 1) {
                rave_effective_samples(psamples, 1, m_node_buf);
                for (int i = (int) depth - 2; i >= 0; i -= 2) {
                        // Update stats for current player.
                        m_path[i]->m_wins += sample.n_wins;
                        m_path[i]->m_sims += sample.n_sims;

                        m_path[i]->m_rwins += sample.n_wins;
                        m_path[i]->m_rsims += sample.n_sims;

                        for (int j = (int) i - 3; j >= 0; j -= 2) {
                                GameNode* cur_player = m_path[j]->find_child(m_path[i]->key);
                                cur_player->m_rsims += sample.n_sims;
                                cur_player->m_rwins += sample.n_wins;
                        }

                        // Update rave stats for the opponent.
                        for (const Move& m: m_node_buf) {
                                GameNode* oppo = m_path[i]->find_child(m.key);
                                const SmallSample& rave = get_rave(psamples, 1, m.x, m.y);
                                oppo->m_rsims += rave.n_sims;
                        }
                }

                rave_effective_samples(psamples, 0, m_node_buf);
                for (int i = (int) depth - 1; i >= 0; i -= 2) {
                        // Update stats for the opponent.
                        m_path[i]->m_sims += sample.n_sims;
                        m_path[i]->m_rsims += sample.n_sims;

                        for (int j = (int) i - 3; j >= 0; j -= 2) {
                                GameNode* oppo = m_path[j]->find_child(m_path[i]->key);
                                oppo->m_rsims += sample.n_sims;
                        }

                        // Update rave stats for current player.
                        for (const Move& m: m_node_buf) {
                                GameNode* cur_player = m_path[i]->find_child(m.key);
                                const SmallSample& rave = get_rave(psamples, 0, m.x, m.y);
                                cur_player->m_rsims += rave.n_sims;
                                cur_player->m_rwins += rave.n_wins;
                        }

                        GameNode* cur_player = m_path[i]->find_child(node.key);
                        cur_player->m_rsims += sample.n_sims;
                        cur_player->m_rwins += sample.n_wins;
                }
        }
}

const MCGameTree::SmallSample& MCGameTree::get_rave(const SmallSample** rave, unsigned p, unsigned x, unsigned y) const
{
        return rave[p][x + y*m_s.num_cols];
}

MCGameTree::SmallSample& MCGameTree::get_rave(unsigned p, unsigned x, unsigned y)
{
        return m_psamp_buf[p][x + y*m_s.num_cols];
}

void MCGameTree::rave_effective_samples(const SmallSample** rave, unsigned p, std::vector<Move>& eff_moves) const
{
        eff_moves.clear();

        for (unsigned y = 0; y < m_s.num_rows; y ++) {
                for (unsigned x = 0; x < m_s.num_cols; x ++) {
                        const SmallSample& rsp = get_rave(rave, p, x, y);
                        if (rsp.n_sims > 0) {
                                eff_moves.push_back(Move(x, y));
                        }
                }
        }
}

void MCGameTree::clear_rave()
{
        for (unsigned i = 0; i < m_s.num_cols*m_s.num_rows; i ++) {
                m_psamp_buf[0][i].a = 0;
                m_psamp_buf[1][i].a = 0;
        }
}

float MCGameTree::win_rate() const
{
        return 1.0f - m_path[0]->m_wins/(float) m_path[0]->m_sims;
}

unsigned MCGameTree::pl() const
{
        return m_path_length;
}

unsigned MCGameTree::max_depth() const
{
        return m_max_depth;
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
        GameNode& selected = mcgt.switch_to_optimal_node(depth, sample_count);
        mcgt.expand_node(selected, depth);

        MCGameTree::Sample sample;
        if (depth == 0) {
                // Warm up.
                while (selected.m_i_chn < selected.m_num_chn) {
                        for (unsigned i = 0; i < 100; i ++) {
                                const MCGameTree::SmallSample** path_samples =
                                                mcgt.sample_at(selected.m_chn[selected.m_i_chn], depth + 1,
                                                sample_count/5, sample);
                                mcgt.back_propagate(sample, path_samples, selected.m_chn[selected.m_i_chn], depth + 1);
                        }
                        selected.m_i_chn ++;
                }
        } else {
                if (selected.m_num_chn == 0) {
                        // Make more samples at this goal state.
                        const MCGameTree::SmallSample** path_samples =
                                        mcgt.sample_at(selected, depth, sample_count, sample);
                        mcgt.back_propagate(sample, path_samples, selected, depth);
                        return ;
                } else {
                        const MCGameTree::SmallSample** path_samples =
                                mcgt.sample_at(selected.m_chn[selected.m_i_chn], depth + 1, sample_count, sample);
                        mcgt.back_propagate(sample, path_samples, selected.m_chn[selected.m_i_chn], depth + 1);
                        if (sample.n_wins < sample.n_sims)
                                selected.m_i_chn ++;
                }
        }
}

void StrategyMCTS::make_move(const State& s, unsigned quality, unsigned time, Move& m) const
{
        if (s.last_move.key == 0XFFFF) {
                // Hard code first move.
                m.set(s.num_cols/2, s.num_rows/2);
                return;
        }

        unsigned sample_count = 500;
        const unsigned SUB_CYCLES = 500;
        const unsigned DEPTH_LIMIT = 12;

        MCGameTree mcgt(s, DEPTH_LIMIT);
        StopWatch watch;
        watch.begin(20000);

        while (watch.check_point() > 0) {
                for (unsigned i = 0; i < SUB_CYCLES; i ++) {
                        ::search(sample_count, mcgt);
                }
                const GameNode& best = mcgt.get_optimal_node();
                m.set(best.m_x, best.m_y);
                std::cout << "B = " << best.beta() << ", " << mcgt << ", Current " << m << std::endl;
                //mcgt.print_score_map(std::cout);
                //std::cout << std::endl;
        }
}

void StrategyMCTS::print(std::ostream& os) const
{
}
