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
        const MonteCarloGameNode&       get_root() const;
        MonteCarloGameNode&             switch_to_optimal_node(unsigned& depth, unsigned step_size);
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
        MonteCarloGameNode              m_root;
        MonteCarloGameNode**            m_path;
        unsigned                        m_path_length = 0;
        unsigned                        m_max_depth;
        std::vector<Move>               m_node_buf;
        std::vector<SampleMove>         m_sample_buf;
        SmallSample*                    m_psamp_buf[2];
};

std::ostream& operator<< (std::ostream& os, const MCGameTree& mcgt)
{
        os << "T = " << mcgt.m_path[0]->get_n_sims() << ", E{X} = " << mcgt.win_rate() << ", PL = " << mcgt.m_path_length << ", ";
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

MCGameTree::~MCGameTree()
{
        delete [] m_path;
        delete [] m_psamp_buf[0];
        delete [] m_psamp_buf[1];
}

const MonteCarloGameNode& MCGameTree::get_root() const
{
        return m_root;
}

void MCGameTree::print_score_map(std::ostream& os) const
{
        for (unsigned y = 0; y < m_s.num_rows; y ++) {
                for (unsigned x = 0; x < m_s.num_cols; x ++) {
                        const MonteCarloGameNode* n = m_root.view_child(Move(x, y).key);
                        if (n)
                                os << n->qi(m_root.get_n_sims()) << "\t";
                                //os << n->rave_win_rate() << "\t";
                                //os << ->win_rate() << "\t";
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
                        if (node->has_next_child())
                                node->reset_iterator();
                        return *node;
                } else if (node->has_next_child() || !node->has_child()) {
                        return *node;
                } else {
                        depth ++;
                        node = &node->get_best_child(m_path[depth - 1]->get_n_sims());
                }
        }
}

const std::vector<MCGameTree::SampleMove>& MCGameTree::expand_node(MonteCarloGameNode& node, unsigned depth)
{
        if (node.is_goal()) {
                // No children can be made at a goal state.
                return m_sample_buf;
        }
        if (node.has_child()) {
                // Expand if child node is not present.
                m_s.expand(node.get_internal_children());
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

        if (node.is_goal()) {
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
        if (depth >= 1) {
                node.update_node_stats(sample.n_wins, sample.n_sims, 0, 0);
        } else {
                node.update_node_stats(0, 0, 0, 0);
        }

        if (depth >= 1) {
                rave_effective_samples(psamples, 1, m_node_buf);

                for (int i = (int) depth - 2; i >= 0; i -= 2) {
                        // Update stats for current player.
                        m_path[i]->update_node_stats(sample.n_wins, sample.n_sims,
                                                     sample.n_wins, sample.n_sims);

                        for (int j = (int) i - 3; j >= 0; j -= 2) {
                                MonteCarloGameNode* cur_player = m_path[j]->get_child(m_path[i]->key);
                                if (cur_player) {
                                        cur_player->update_node_stats(0, 0, sample.n_wins, sample.n_sims);
                                }
                        }

                        // Update rave stats for the opponent.
                        for (const Move& m: m_node_buf) {
                                MonteCarloGameNode* oppo = m_path[i]->get_child(m.key);
                                const SmallSample& rave = get_rave(psamples, 0, m.x, m.y);
                                if (oppo) {
                                        oppo->update_node_stats(0, 0, 0, rave.n_sims);
                                }
                        }
                }

                rave_effective_samples(psamples, 0, m_node_buf);
                for (int i = (int) depth - 1; i >= 0; i -= 2) {
                        // Update stats for the opponent.
                        m_path[i]->update_node_stats(0, sample.n_sims,
                                                     0, sample.n_sims);

                        for (int j = (int) i - 3; j >= 0; j -= 2) {
                                MonteCarloGameNode* oppo = m_path[j]->get_child(m_path[i]->key);
                                if (oppo) {
                                        oppo->update_node_stats(0, 0, 0, sample.n_sims);
                                }
                        }

                        // Update rave stats for current player.
                        for (const Move& m: m_node_buf) {
                                MonteCarloGameNode* cur_player = m_path[i]->get_child(m.key);
                                const SmallSample& rave = get_rave(psamples, 0, m.x, m.y);
                                if (cur_player) {
                                        cur_player->update_node_stats(0, 0, rave.n_wins, rave.n_sims);
                                }
                        }

                        MonteCarloGameNode* cur_player = m_path[i]->get_child(node.key);
                        if (cur_player) {
                                cur_player->update_node_stats(0, 0, sample.n_wins, sample.n_sims);
                        }
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
        return 1.0f - m_path[0]->win_rate();
}

unsigned MCGameTree::pl() const
{
        return m_path_length;
}

unsigned MCGameTree::max_depth() const
{
        return m_max_depth;
}

static void search(unsigned sample_count, MCGameTree& mcgt)
{
        unsigned depth;
        MonteCarloGameNode& selected = mcgt.switch_to_optimal_node(depth, sample_count);
        const std::vector<MCGameTree::SampleMove>& smoves = mcgt.expand_node(selected, depth);

        MCGameTree::Sample sample;
        if (depth == 0) {
                // Warm up.
                while (selected.has_next_child()) {
                        MonteCarloGameNode& node = selected.get_next_child();
                        for (unsigned i = 0; i < 100; i ++) {
                                const MCGameTree::SmallSample** path_samples =
                                                mcgt.sample_at(node, smoves, depth + 1, sample_count/5, sample);
                                mcgt.back_propagate(sample, path_samples, node, depth + 1);
                        }
                        //const MCGameTree::SmallSample** path_samples =
                        //                mcgt.sample_at(selected.m_chn[selected.m_i_chn], depth + 1,
                        //                sample_count, sample);
                        //mcgt.back_propagate(sample, path_samples, selected.m_chn[selected.m_i_chn], depth + 1);
                }
        } else {
                if (!selected.has_child()) {
                        // Make more samples at this goal state.
                        const MCGameTree::SmallSample** path_samples =
                                        mcgt.sample_at(selected, smoves, depth, sample_count, sample);
                        mcgt.back_propagate(sample, path_samples, selected, depth);
                        return ;
                } else {
                        MonteCarloGameNode& node = selected.get_next_child();
                        const MCGameTree::SmallSample** path_samples =
                                mcgt.sample_at(node, smoves, depth + 1, sample_count, sample);
                        mcgt.back_propagate(sample, path_samples, node, depth + 1);
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
                const MonteCarloGameNode& best = mcgt.get_root().get_most_simulated_child();
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
