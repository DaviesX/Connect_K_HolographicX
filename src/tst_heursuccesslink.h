#ifndef TST_HEURSUCCESSLINK_H
#define TST_HEURSUCCESSLINK_H

#include <iostream>
#include <tst_state.h>
#include <heursuccesslink.h>


inline static int tst_heursuccesslink()
{
        State s = tst_state_create_sample2();

        std::cout << "Preview: " << std::endl;
        std::cout << s << std::endl;

        HeuristicSuccessLink h;
        h.load_state(s);

        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        if (s.is(x, y) == State::NO_PIECE) {
                                std::cout << h.evaluate(s, Move(x, y), State::AI_PIECE) << "\t";
                        } else {
                                std::cout << "-INFINITY" << "\t";
                        }
                }
                std::cout << std::endl;
        }
        return 0;
}


#endif
