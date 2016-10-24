#ifndef TST_HEURSUCCESSLINK_H
#define TST_HEURSUCCESSLINK_H

#include <iostream>
#include <tst_state.h>
#include <heurchessdeg.h>


inline static int tst_heurchessdeg()
{
        State s = tst_state_create_sample3();

        std::cout << "Preview: " << std::endl;
        std::cout << s << std::endl;

        HeuristicChessDegree h;
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
