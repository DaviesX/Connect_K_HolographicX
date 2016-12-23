#ifndef TST_STRATEGYMCTS_H
#define TST_STRATEGYMCTS_H

#include <vector>
#include "tst_state.h"
#include "state.h"
#include "strategymcts.h"


static int tst_strategymcts()
{
        std::vector<State> states;
        states.push_back(::tst_state_create_sample12());
        StrategyMCTS strategy;

        Move m;
        for (unsigned i = 0; i < states.size(); i ++) {
                std::cout << "Preview state: " << i + 1 << std::endl;
                std::cout << states[i] << std::endl;

                State copy = states[i];
                strategy.load_state(states[i]);
                strategy.make_move(states[i], 2, 20000, m);

                std::cout << "Move made " << m << std::endl;
                std::cout << "State is unchanged " << (copy == states[i]) << std::endl;
        }
        return EXIT_SUCCESS;
}

#endif // TST_STRATEGYMCTS_H
