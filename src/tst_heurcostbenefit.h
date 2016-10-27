#ifndef TST_HEUR_COST_BENEFIT_H
#define TST_HEUR_COST_BENEFIT_H

#include <iostream>
#include <tst_state.h>
#include <heurcostbenefit.h>

static inline int tst_heurcostbenefit()
{
        // State s = tst_state_create_sample();
        State s = tst_state_create_sample3();

        std::cout << "Preview: " << std::endl;
        std::cout << s << std::endl;

        HeuristicCostBenefit h;
        h.load_state(s);
#if 1
        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        if (s.is(x, y) == State::NO_PIECE) {
                                std::cout << h.evaluate(s, Move(x, y), State::AI_PIECE) << "\t";
                        } else {
                                std::cout << "NOMAN" << "\t";
                        }
                }
                std::cout << std::endl;
        }
#else
        std::cout << "Move value ";
        std::cout << h.evaluate(s, Move(3, 3), State::AI_PIECE) << "\t";
#endif
        return 0;
}

#endif
