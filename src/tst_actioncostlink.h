#ifndef TST_ACTIONCOSTLINK_H
#define TST_ACTIONCOSTLINK_H

#include <iostream>
#include <cstdlib>
#include <state.h>
#include <istrategy.h>
#include <strategydfs.h>
#include <actioncostlink.h>


static State tst_create_sample_state(const IStrategy& strategy)
{
        State s(7, 5, false, 
                Move(2, 3), 5, 5000,
                strategy.get_gxy(), strategy.get_fxy());
        s.is(0, 3, State::HUMAN_PIECE); 
        s.is(2, 3, State::HUMAN_PIECE); 
        s.is(3, 3, State::HUMAN_PIECE); 
        return s;
}

static int tst_actioncostlink()
{
        StrategyDFS strategy;
        const State& s = tst_create_sample_state(strategy);
        ActionCostLink cost;
        std::cout << cost.evaluate(s, 4, 3) << std::endl;
        return EXIT_SUCCESS;
}

#endif
