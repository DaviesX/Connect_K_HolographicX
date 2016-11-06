#ifndef TST_HEUR_COST_BENEFIT_H
#define TST_HEUR_COST_BENEFIT_H

#include <iostream>
#include "tst_state.h"
#include "heurcostbenefit.h"

static inline int tst_heurcostbenefit()
{
        State s = tst_state_create_sample7();

        s.set_move(5, 2, State::AI_PIECE);
        s.set_move(2, 5, State::HUMAN_PIECE);

        std::cout << "Preview: " << std::endl;
        std::cout << s << std::endl;

        HeuristicCostBenefit h;
        h.load_state(s);
        h.try_move(s, Move(5, 2));
        h.try_move(s, Move(2, 5));
#if 1
        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        if (s.is(x, y) == State::NO_PIECE) {
                                s.set_move(x, y, State::AI_PIECE);
                                std::cout << h.evaluate(s, Move(x, y)) << "\t";
                                s.set_move(x, y, State::NO_PIECE);
                        } else {
                                std::cout << "NOMAN" << "\t";
                        }
                }
                std::cout << std::endl;
        }
#else
        std::cout << "Move value ";
        s.set_move(0, 7, State::AI_PIECE);
        std::cout << h.evaluate(s, Move(0, 7)) << "\t";
#endif
        return 0;
}

#endif
