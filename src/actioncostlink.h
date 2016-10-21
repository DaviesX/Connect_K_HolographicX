#ifndef ACTIONCOSTLINK_H
#define ACTIONCOSTLINK_H

#include <ostream>
#include <iactioncost.h>


/*
 * <ActionCostLink> A linked based cost evaluation method.
 */
class ActionCostLink: public IActionCost
{
        friend std::ostream& operator<<(std::ostream& os, const ActionCostLink& ac);
public:
        ActionCostLink();
        ~ActionCostLink() override;
        void            load_state(const State& s) override;
        void            accept(const State& s, float score, unsigned x, unsigned y) override;
        float           evaluate(const State& s, unsigned x, unsigned y) override;
        void            print(std::ostream& os) const;
private:
        bool            is_markable(const State& s, const unsigned x, const unsigned y, const int who) const;
        void            mark(const unsigned x, const unsigned y);
        void            new_marker();
        float           score_links_for(const State& s, int who);

        unsigned        trace(const State& s, const int who, int x, int y, const int dx, const int dy);

        float           m_cur_score = 0; 
        unsigned*       m_board = nullptr;
        unsigned        m_marker = 1;
        unsigned        m_w = 0;
        unsigned        m_h = 0;
};

std::ostream& operator<<(std::ostream& os, const ActionCostLink& ac);


#endif  // ACTIONCOSTLINK_H
