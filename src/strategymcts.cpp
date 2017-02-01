#include <limits>
#include <algorithm>
#include <vector>
#include <map>
#include <iostream>
#include "state.h"
#include "gamenode.h"
#include "stopwatch.h"
#include "strategymcts.h"

#define WITH_RAVE
//#define TEST



StrategyMCTS::StrategyMCTS()
{
}

static MonteCarloGameNode* expand_state(const State& s, std::vector<Move>& buf, bool to_nodes)
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
                std::sort(buf.begin(), buf.end());
        }

        if (to_nodes && !buf.empty()) {
                MonteCarloGameNode* nodes = new MonteCarloGameNode [buf.size()];
                for (unsigned i = 0; i < buf.size(); i ++) {
                        nodes[i].x = buf[i].x;
                        nodes[i].y = buf[i].y;
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

        struct SampleMove: Move
        {
                float   p;
        };

        MCGameTree(const State& s, unsigned max_depth);
        ~MCGameTree();
        MonteCarloGameNode&                       switch_to_optimal_node(unsigned& depth, unsigned step_size);
        MonteCarloGameNode&                       get_best_node() const;
        MonteCarloGameNode&                       get_optimal_node() const;
        const std::vector<SampleMove>&  expand_node(MonteCarloGameNode& node, unsigned depth);
        const SmallSample**             sample_at(const MonteCarloGameNode& node, const std::vector<SampleMove>& smoves,
                                                  unsigned depth, unsigned sample_count, Sample& sample);
        void                            back_propagate(const Sample& sample, const SmallSample** psamples,
                                                       MonteCarloGameNode& nodes, unsigned depth);

        SmallSample&                    get_rave(unsigned p, unsigned x, unsigned y) const;
        const SmallSample&              get_rave(const SmallSample** rave, unsigned p, unsigned x, unsigned y) const;
        void                            rave_effective_samples(const SmallSample** rave, unsigned p,
                                                               std::vector<Move>& eff_moves) const;
        void                            best_rave(unsigned p, Move& m) const;
        void                            clear_rave();
        void                            print_rave_map(std::ostream& os, unsigned p, const SmallSample** rave) const;
        float                           win_rate() const;
        unsigned                        pl() const;
        unsigned                        max_depth() const;
        void                            print_score_map(std::ostream& os) const;
private:
        State                           m_s;
        MonteCarloGameNode                        m_root;
        MonteCarloGameNode**                      m_path;
        unsigned                        m_path_length = 0;
        unsigned                        m_max_depth;
        std::vector<Move>               m_node_buf;
        std::vector<SampleMove>         m_sample_buf;
        SmallSample*                    m_psamp_buf[2];
};

std::ostream& operator<< (std::ostream& os, const MCGameTree& mcgt)
{
        os << "T = " << mcgt.m_path[0]->m_sims << ", E{X} = " << mcgt.win_rate() << ", PL = " << mcgt.m_path_length << ", ";
        for (unsigned i = 1; i < mcgt.m_path_length; i ++)
                os << Move(mcgt.m_path[i]->x, mcgt.m_path[i]->y) << ", ";
        return os;
}

MCGameTree::MCGameTree(const State& s, unsigned max_depth):
        m_s(s), m_max_depth(std::min(s.num_left, max_depth))
{
        m_path = new MonteCarloGameNode* [m_max_depth + 1];

        m_psamp_buf[0] = new SmallSample [s.num_cols*s.num_rows];
        m_psamp_buf[1] = new SmallSample [s.num_cols*s.num_rows];

        m_sample_buf.reserve(s.num_left);
        m_node_buf.reserve(s.num_left);
}

static void MCGameTree_free(MonteCarloGameNode* node)
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
                        const MonteCarloGameNode* n = m_root.find_child(Move(x, y).key);
                        if (n)
                                os << n->qi(m_root.m_sims) << "\t";
                                //os << (float) n->m_rwins/n->m_rsims << "\t";
                                //os << (float) n->m_wins/n->m_sims << "\t";
                        else
                                os << "NAN" << "\t";
                }
                os << std::endl;
        }
}

MonteCarloGameNode& MCGameTree::switch_to_optimal_node(unsigned& depth, unsigned step_size)
{
        MonteCarloGameNode* node = &m_root;
        depth = 0;

        // Reset all the moves.
        for (unsigned i = 1; i < m_path_length; i ++) {
                m_s.set_move(m_path[i]->x, m_path[i]->y, State::NO_PIECE);
        }

        // Switch to the optimal branch.
        m_path_length = 0;
        while (true) {
                if (node->key != 0xFFFF)
                        m_s.set_move(node->x, node->y, ((depth & 1)) == 1 ? State::AI_PIECE : State::HUMAN_PIECE);
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

MonteCarloGameNode& MCGameTree::get_best_node() const
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

MonteCarloGameNode& MCGameTree::get_optimal_node() const
{
        return *m_path[1];
}

const std::vector<MCGameTree::SampleMove>& MCGameTree::expand_node(MonteCarloGameNode& node, unsigned depth)
{
        if (node.m_sims > 0 && node.m_wins >= node.m_sims) {
                // No children can be made at a goal state.
                return m_sample_buf;
        }
        if (node.m_num_chn == 0) {
                node.m_chn = ::expand_state(m_s, m_node_buf, true);
                node.m_num_chn = m_node_buf.size();
        }

        // Reconstruct sample moves.
        m_sample_buf.resize(m_node_buf.size());
        for (unsigned i = 0; i < m_node_buf.size(); i ++) {
                m_sample_buf[i].x = m_node_buf[i].x;
                m_sample_buf[i].y = m_node_buf[i].y;
                m_sample_buf[i].p = 0.0f;
        }
        std::sort(m_sample_buf.begin(), m_sample_buf.end());
        return m_sample_buf;
}

const MCGameTree::SmallSample** MCGameTree::sample_at(const MonteCarloGameNode& node, const std::vector<SampleMove>& smoves,
                                                      unsigned depth, unsigned sample_count, Sample& sample)
{
        clear_rave();

        if (node.m_sims > 0 && node.m_wins >= node.m_sims) {
                sample.n_sims = sample_count;
                sample.n_wins = sample_count;

                return const_cast<const SmallSample**>(m_psamp_buf);
        }

        int player = (depth & 1) == 1 ? State::AI_PIECE : State::HUMAN_PIECE;
        m_s.set_move(node.x, node.y, player);
        if (m_s.is_goal_for(Move(node.x, node.y), player)) {
                sample.n_sims = sample_count;
                sample.n_wins = sample_count;

                m_s.set_move(node.x, node.y, State::NO_PIECE);
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
#ifdef WITH_RAVE
                        // Update RAVE stats for the current player.
                        for (unsigned l = 1; l <= j; l += 2) {
                                get_rave(0, moves[l].x, moves[l].y).n_wins ++;
                        }
#endif
                } else {
                        //sample.n_losses ++;
#ifdef WITH_RAVE
                        // Update RAVE stats for the opponent.
                        for (unsigned l = 0; l <= j; l += 2) {
                                get_rave(1, moves[l].x, moves[l].y).n_wins ++;
                        }
#endif
                }

                // Clear previous playout and update RAVE.
                for (unsigned l = 0; l <= j; l ++) {
#ifdef WITH_RAVE
                        unsigned p = (l + 1) & 1;
                        get_rave(p, moves[l].x, moves[l].y).n_sims ++;
#endif
                        m_s.set_move(moves[l].x, moves[l].y, State::NO_PIECE);
                }
        }

        // Eliminate losing move.
        Move worse;
        best_rave(1, worse);
        if (m_s.is_steady_goal_for(worse, opponent_of(player))) {
                if (!m_s.is_steady_goal_for(Move(node.x, node.y), player)) {
                        sample.n_wins = 0;
                        //sample.n_losses = sample_count;
                }
        }

        m_s.set_move(node.x, node.y, State::NO_PIECE);

        delete [] moves;
        return const_cast<const SmallSample**>(m_psamp_buf);
}

void MCGameTree::back_propagate(const Sample& sample, const SmallSample** psamples,
                                MonteCarloGameNode& node, unsigned depth)
{
        node.m_sims += sample.n_sims;
        node.m_wins += sample.n_wins;

        if (depth >= 1) {
#ifdef WITH_RAVE
                rave_effective_samples(psamples, 1, m_node_buf);
#endif
                for (int i = (int) depth - 2; i >= 0; i -= 2) {
                        // Update stats for current player.
                        m_path[i]->m_wins += sample.n_wins;
                        m_path[i]->m_sims += sample.n_sims;
#ifdef WITH_RAVE
                        m_path[i]->m_rwins += sample.n_wins;
                        //m_path[i]->m_rlosses += sample.n_losses;
                        m_path[i]->m_rsims += sample.n_sims;

                        for (int j = (int) i - 3; j >= 0; j -= 2) {
                                MonteCarloGameNode* cur_player = m_path[j]->find_child(m_path[i]->key);
                                if (cur_player) {
                                        cur_player->m_rsims += sample.n_sims;
                                        cur_player->m_rwins += sample.n_wins;
                                        //cur_player->m_rlosses += sample.n_losses;
                                }
                        }

                        // Update rave stats for the opponent.
                        for (const Move& m: m_node_buf) {
                                MonteCarloGameNode* oppo = m_path[i]->find_child(m.key);
                                const SmallSample& rave = get_rave(psamples, 1, m.x, m.y);
                                //const SmallSample& oppo_rave = get_rave(psamples, 0, m.x, m.y);
                                if (oppo) {
                                        oppo->m_rsims += rave.n_sims;
                                        oppo->m_rwins += rave.n_wins;
                                        //oppo->m_rlosses += oppo_rave.n_wins;
                                }
                        }
#endif
                }
#ifdef WITH_RAVE
                rave_effective_samples(psamples, 0, m_node_buf);
#endif
                for (int i = (int) depth - 1; i >= 0; i -= 2) {
                        // Update stats for the opponent.
                        m_path[i]->m_sims += sample.n_sims;

#ifdef WITH_RAVE
                        //m_path[i]->m_rwins += sample.n_losses;
                        //m_path[i]->m_rlosses += sample.n_wins;
                        m_path[i]->m_rsims += sample.n_sims;

                        for (int j = (int) i - 3; j >= 0; j -= 2) {
                                MonteCarloGameNode* oppo = m_path[j]->find_child(m_path[i]->key);
                                if (oppo) {
                                        oppo->m_rsims += sample.n_sims;

                                        oppo->m_rsims += sample.n_sims;
                                        //oppo->m_rwins += sample.n_losses;
                                }
                        }

                        // Update rave stats for current player.
                        for (const Move& m: m_node_buf) {
                                MonteCarloGameNode* cur_player = m_path[i]->find_child(m.key);
                                const SmallSample& rave = get_rave(psamples, 0, m.x, m.y);
                                if (cur_player) {
                                        cur_player->m_rsims += rave.n_sims;
                                        cur_player->m_rwins += rave.n_wins;
                                }
                        }

                        MonteCarloGameNode* cur_player = m_path[i]->find_child(node.key);
                        if (cur_player) {
                                cur_player->m_rsims += sample.n_sims;
                                cur_player->m_rwins += sample.n_wins;
                        }
#endif
                }
        }
}

const MCGameTree::SmallSample& MCGameTree::get_rave(const SmallSample** rave, unsigned p, unsigned x, unsigned y) const
{
        return rave[p][x + y*m_s.num_cols];
}

MCGameTree::SmallSample& MCGameTree::get_rave(unsigned p, unsigned x, unsigned y) const
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

void MCGameTree::best_rave(unsigned p, Move& m) const
{
        float w = 0.0f;
        for (unsigned y = 0; y < m_s.num_rows; y ++) {
                for (unsigned x = 0; x < m_s.num_cols; x ++) {
                        const SmallSample& rsp = get_rave(p, x, y);
                        float win_rate = (float) rsp.n_wins/rsp.n_sims;
                        if (win_rate > w) {
                                w = win_rate;
                                m.set(x, y);
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

void MCGameTree::print_rave_map(std::ostream& os, unsigned p, const SmallSample** rave) const
{
        for (unsigned y = 0; y < m_s.num_rows; y ++) {
                for (unsigned x = 0; x < m_s.num_cols; x ++) {
                        const SmallSample& rsp = get_rave(rave, p, x, y);
                        os << (float) rsp.n_wins/rsp.n_sims << "\t";
                }
                os << std::endl;
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

static const MonteCarloGameNode& best_action(const std::vector<MonteCarloGameNode>& actions)
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
        MonteCarloGameNode& selected = mcgt.switch_to_optimal_node(depth, sample_count);
        const std::vector<MCGameTree::SampleMove>& smoves = mcgt.expand_node(selected, depth);

        MCGameTree::Sample sample;
        if (depth == 0) {
                // Warm up.
                while (selected.m_i_chn < selected.m_num_chn) {
                        for (unsigned i = 0; i < 100; i ++) {
                                const MCGameTree::SmallSample** path_samples =
                                                mcgt.sample_at(selected.m_chn[selected.m_i_chn], smoves,
                                                depth + 1, sample_count/5, sample);
                                mcgt.back_propagate(sample, path_samples, selected.m_chn[selected.m_i_chn], depth + 1);
                        }
                        //const MCGameTree::SmallSample** path_samples =
                        //                mcgt.sample_at(selected.m_chn[selected.m_i_chn], depth + 1,
                        //                sample_count, sample);
                        //mcgt.back_propagate(sample, path_samples, selected.m_chn[selected.m_i_chn], depth + 1);
                        selected.m_i_chn ++;
                }
        } else {
                if (selected.m_num_chn == 0) {
                        // Make more samples at this goal state.
                        const MCGameTree::SmallSample** path_samples =
                                        mcgt.sample_at(selected, smoves, depth, sample_count, sample);
                        mcgt.back_propagate(sample, path_samples, selected, depth);
                        return ;
                } else {
                        const MCGameTree::SmallSample** path_samples =
                                mcgt.sample_at(selected.m_chn[selected.m_i_chn], smoves, depth + 1, sample_count, sample);
                        mcgt.back_propagate(sample, path_samples, selected.m_chn[selected.m_i_chn], depth + 1);
                        if (sample.n_wins < sample.n_sims)
                                selected.m_i_chn ++;
                }
        }
}

void StrategyMCTS::make_move(const State& s, unsigned quality, unsigned time, Move& m) const
{
#ifdef TEST
        MCGameTree mcgt(s, s.num_left);

        unsigned depth, sample_count = 100;
        GameNode& selected = mcgt.switch_to_optimal_node(depth, sample_count);
        mcgt.expand_node(selected, depth);

        GameNode& target = *selected.find_child(Move(5, 4).key);

        MCGameTree::Sample sample;
        const MCGameTree::SmallSample** path_samples =
                mcgt.sample_at(target, depth + 1, sample_count, sample);
        mcgt.back_propagate(sample, path_samples, target, depth + 1);
        mcgt.print_rave_map(std::cout, 1, path_samples);
        std::cout << std::endl;
        mcgt.print_score_map(std::cout);
        std::cout << std::endl;
#else
        if (s.last_move.key == 0XFFFF) {
                // Hard code first move.
                m.set(s.num_cols/2, s.num_rows/2);
                return;
        }

        unsigned sample_count = 500;
        const unsigned SUB_CYCLES = 1000;
        const unsigned DEPTH_LIMIT = s.num_left;

        MCGameTree mcgt(s, DEPTH_LIMIT);
        StopWatch watch;
        watch.begin(20000);

        while (watch.check_point() > 0) {
                for (unsigned i = 0; i < SUB_CYCLES; i ++) {
                        ::search(sample_count, mcgt);
                }
                const MonteCarloGameNode& best = mcgt.get_best_node();
                m.set(best.x, best.y);
                std::cout << "B = " << best.beta() << ", " << mcgt << ", Current " << m << std::endl;
                //mcgt.print_score_map(std::cout);
                //std::cout << std::endl;
        }
#endif
}

void StrategyMCTS::print(std::ostream& os) const
{
}
