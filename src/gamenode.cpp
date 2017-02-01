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

MonteCarloGameNode::MonteCarloGameNode(unsigned char x, unsigned char y):
        Move(x, y)
{
}

float MonteCarloGameNode::beta() const
{
        const float b = 0.01f;
        const float n_hat = static_cast<float>(m_rsims);
        const float n = static_cast<float>(m_sims);
        return n_hat/(n + n_hat + 4.0f*b*b*n*n_hat);
}

float MonteCarloGameNode::qi(unsigned n_sims) const
{
        const float c = 5.0f;
        float b = beta();
        float win_rate = static_cast<float>(m_wins)/m_sims;
        float rave_win_rate = static_cast<float>(m_rwins)/m_rsims;
        return (1.0f - b)*win_rate + b*rave_win_rate +
                std::sqrt(c*std::log(static_cast<float>(n_sims))/m_sims);
}

MonteCarloGameNode* MonteCarloGameNode::find_child(unsigned short key) const
{
        int l = 0, h = m_num_chn - 1;
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

std::ostream& operator<<(std::ostream& os, const MonteCarloGameNode& node)
{
        os << static_cast<float>(node.m_wins)/node.m_sims;
        return os;
}
