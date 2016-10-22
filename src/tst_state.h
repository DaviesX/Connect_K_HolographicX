#ifndef TST_STATE_H
#define TST_STATE_H

#include <iostream>
#include <state.h>

static inline State tst_state_create_sample()
{
        State s(7, 7, true, Move(3, 4), 5, 5000);

        s.set_move(1, 1, State::AI_PIECE);
        s.set_move(2, 2, State::AI_PIECE);
        s.set_move(4, 4, State::AI_PIECE);
        s.set_move(5, 5, State::AI_PIECE);

        s.set_move(4, 2, State::AI_PIECE);
        s.set_move(4, 3, State::AI_PIECE);
        s.set_move(4, 4, State::AI_PIECE);
        s.set_move(4, 5, State::AI_PIECE);

        s.set_move(1, 4, State::HUMAN_PIECE);
        s.set_move(2, 4, State::HUMAN_PIECE);
        s.set_move(3, 5, State::HUMAN_PIECE);
        s.set_move(4, 6, State::HUMAN_PIECE);

        s.set_move(2, 3, State::HUMAN_PIECE);
        s.set_move(3, 2, State::HUMAN_PIECE);
        s.set_move(4, 1, State::HUMAN_PIECE);

        s.set_move(5, 2, State::HUMAN_PIECE);

        return s;
}

static inline int tst_state() 
{
        State s = ::tst_state_create_sample();
        std::cout << "Preview" << std::endl;
        std::cout << s << std::endl;

        std::cout << "Trying step (3, 3)" << std::endl;
        s.push_move(3, 3, State::AI_PIECE, 5.0);
        std::cout << s << std::endl;

        std::cout << "Should be goal for AI" << std::endl;
        std::cout << s.is_goal_for(State::AI_PIECE) << std::endl;
}


#endif
