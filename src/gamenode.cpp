#include <cassert>
#include "constants.h"
#include "gamenode.h"


// HeuristicGameNode
HeuristicGameNode::HeuristicGameNode(unsigned char x, unsigned char y, int score):
        Move(x, y), score(score)
{
}

bool HeuristicGameNode::operator < (const HeuristicGameNode& action) const
{
        return score < action.score;
}

bool HeuristicGameNode::operator > (const HeuristicGameNode& action) const
{
        return score > action.score;
}

// MonteCarloGameNode
MonteCarloGameNode::MonteCarloGameNode():
        Move(0xFF, 0xFF)
{
}

MonteCarloGameNode::MonteCarloGameNode(const Move& m):
        Move(m.x, m.y)
{
}

unsigned MonteCarloGameNode::get_n_wins() const
{
        return m_wins;
}

unsigned MonteCarloGameNode::get_n_sims() const
{
        return m_sims;
}

unsigned MonteCarloGameNode::get_r_wins() const
{
        return m_rwins;
}

unsigned MonteCarloGameNode::get_r_sims() const
{
        return m_rsims;
}

bool MonteCarloGameNode::has_simulated() const
{
        return m_sims != 0;
}

bool MonteCarloGameNode::is_goal() const
{
        return has_simulated() && (m_wins == m_sims);
}

float MonteCarloGameNode::win_rate() const
{
        return static_cast<float>(get_n_wins())/static_cast<float>(get_n_sims());
}

float MonteCarloGameNode::rave_win_rate() const
{
        return static_cast<float>(get_r_wins())/static_cast<float>(get_r_sims());
}

void MonteCarloGameNode::update_node_stats(unsigned more_wins, unsigned more_sims,
                                           unsigned more_rwins, unsigned more_rsims)
{
        m_wins += more_wins;
        m_sims += more_sims;
        m_rwins += more_rwins;
        m_rsims += more_rsims;
}

float MonteCarloGameNode::beta() const
{
        float b = constants::b;
        const float n_hat = static_cast<float>(m_rsims);
        const float n = static_cast<float>(m_sims);
        return n_hat/(n + n_hat + 4.0f*b*b*n*n_hat);
}

float MonteCarloGameNode::qi(unsigned t) const
{
        float b = beta();
        return (1.0f - b)*win_rate() + b*rave_win_rate() +
                std::sqrt(constants::c*std::log(static_cast<float>(t))/m_sims);
}

MonteCarloGameNode* MonteCarloGameNode::get_child(unsigned short key)
{
        return const_cast<MonteCarloGameNode*>(view_child(key));
}

const MonteCarloGameNode* MonteCarloGameNode::view_child(unsigned short key) const
{
        int l = 0, h = m_chn.size() - 1;
        while (l <= h) {
                int m = l + static_cast<int>(static_cast<unsigned>(h - l) >> 1);
                if (key == m_chn[m].key)
                        return &m_chn[m];
                else if (key < m_chn[m].key)
                        h = m - 1;
                else
                        l = m + 1;
        }
        return nullptr;
}

MonteCarloGameNode& MonteCarloGameNode::get_next_child()
{
        return m_chn[m_i_chn ++];
}

bool MonteCarloGameNode::has_next_child() const
{
        return m_i_chn < m_chn.size();
}

bool MonteCarloGameNode::has_child() const
{
        return !m_chn.empty();
}

void MonteCarloGameNode::reset_iterator()
{
        m_i_chn = 0;
}

std::vector<MonteCarloGameNode>& MonteCarloGameNode::get_internal_children()
{
        return m_chn;
}

MonteCarloGameNode& MonteCarloGameNode::get_best_child(unsigned t)
{
        assert(m_chn.size() >= 1);

        unsigned i_best = 0;
        float best = 0.0f;
        for (unsigned i = 0; i < m_chn.size(); i ++) {
                float s = m_chn[i].qi(t);
                if (s > best) {
                        i_best = i;
                        best = s;
                }
        }
        return m_chn[i_best];
}

const MonteCarloGameNode& MonteCarloGameNode::get_most_simulated_child() const
{
        assert(m_chn.size() >= 1);

        unsigned i_most = 0;
        unsigned most = 0;
        for (unsigned i = 0; i < m_chn.size(); i ++) {
                unsigned s = get_n_sims();
                if (s > most) {
                        i_most = i;
                        most = s;
                }
        }
        return m_chn[i_most];
}

std::ostream& operator<<(std::ostream& os, const MonteCarloGameNode& node)
{
        os << node.win_rate();
        return os;
}
