#ifndef ACTIONCOSTLINK_H
#define ACTIONCOSTLINK_H

#include <iactioncost.h>


/*
 * <ActionCostLink> A linked based cost evaluation method.
 */
class ActionCostLink: public IActionCost
{
public:
        ActionCostLink();
        float           evaluate(const State& s, unsigned x, unsigned y) override;
        void            print_dbg_info() const override;
private:
        bool            is_markable(const State& s, const unsigned x, const unsigned y, const int who) const;
        void            mark(const unsigned x, const unsigned y);
        void            new_marker();

        unsigned        trace(const State& s, const int who, int x, int y, const int dx, const int dy);
        float           evaluate(const State& s, const int who);

        unsigned*       m_board = nullptr;
        unsigned        m_marker = 1;
        unsigned        m_w = 0;
        unsigned        m_h = 0;
};

#endif  // ACTIONCOSTLINK_H
