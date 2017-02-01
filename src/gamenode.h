#ifndef GAMENODE_H
#define GAMENODE_H

#include <cmath>
#include "move.h"


class HeuristicGameNode: public Move
{
public:
        HeuristicGameNode(unsigned char x, unsigned char y, int score);

        bool operator < (const HeuristicGameNode& action) const;
        bool operator > (const HeuristicGameNode& action) const;
public:
        float score;
};

class MonteCarloGameNode: public Move
{
public:
        MonteCarloGameNode();
        MonteCarloGameNode(unsigned char x, unsigned char y);

        float beta() const;
        float qi(unsigned n_sims) const;
        MonteCarloGameNode* find_child(unsigned short key) const;

        // Childrens are sorted in key order.
        unsigned char           m_num_chn = 0;
        unsigned char           m_i_chn = 0;
        MonteCarloGameNode*     m_chn = nullptr;

        // Statistics.
        unsigned        m_wins = 0;
        unsigned        m_sims = 0;
        unsigned        m_rwins = 0;
        unsigned        m_rsims =  0;

};

std::ostream& operator<<(std::ostream& os, const MonteCarloGameNode& node);


#endif // GAMENODE_H
