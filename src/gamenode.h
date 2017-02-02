#ifndef GAMENODE_H
#define GAMENODE_H

#include <cmath>
#include <vector>
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
        friend std::ostream& operator<<(std::ostream& os, const MonteCarloGameNode& node);
public:
        MonteCarloGameNode();
        MonteCarloGameNode(const Move& m);

        float                                   beta() const;
        float                                   qi(unsigned t) const;

        unsigned                                get_n_wins() const;
        unsigned                                get_n_sims() const;
        unsigned                                get_r_wins() const;
        unsigned                                get_r_sims() const;
        bool                                    has_simulated() const;
        bool                                    is_goal() const;

        float                                   win_rate() const;
        float                                   rave_win_rate() const;

        void                                    update_node_stats(unsigned more_wins, unsigned more_sims,
                                                                  unsigned more_rwins, unsigned more_rsims);

        MonteCarloGameNode*                     get_child(unsigned short key);
        const MonteCarloGameNode*               view_child(unsigned short key) const;
        MonteCarloGameNode&                     get_next_child();
        bool                                    has_next_child() const;
        bool                                    has_child() const;
        void                                    reset_iterator();
        std::vector<MonteCarloGameNode>&        get_internal_children();

        MonteCarloGameNode&                     get_best_child(unsigned t);
        const MonteCarloGameNode&               get_most_simulated_child() const;
private:
        // Childrens are sorted in key order.
        unsigned char                           m_i_chn = 0;
        std::vector<MonteCarloGameNode>         m_chn;

        // Statistics.
        unsigned        m_wins = 0;
        unsigned        m_sims = 0;
        unsigned        m_rwins = 0;
        unsigned        m_rsims = 0;
};

std::ostream& operator<<(std::ostream& os, const MonteCarloGameNode& node);


#endif // GAMENODE_H
