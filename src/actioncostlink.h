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
        float evaluate(const State& s, const Move& m) const override;
};

#endif  // ACTIONCOSTLINK_H
