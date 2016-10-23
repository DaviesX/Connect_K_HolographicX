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

static inline State tst_state_create_sample2()
{
        State s(7, 7, true, Move(4, 3), 5, 5000);
        s.set_move(3, 3, State::HUMAN_PIECE);
        return s;
}

static inline State tst_state_create_sample3()
{
        State s(8, 8, true, Move(7, 2), 5, 5000);

        s.set_move(3, 4, State::AI_PIECE);
        s.set_move(4, 4, State::AI_PIECE);
        s.set_move(4, 3, State::AI_PIECE);
        s.set_move(4, 2, State::AI_PIECE);

        s.set_move(5, 2, State::AI_PIECE);
        s.set_move(6, 1, State::AI_PIECE);

        s.set_move(2, 5, State::HUMAN_PIECE);
        s.set_move(4, 5, State::HUMAN_PIECE);
        s.set_move(3, 3, State::HUMAN_PIECE);
        s.set_move(3, 2, State::HUMAN_PIECE);

        s.set_move(4, 1, State::HUMAN_PIECE);
        s.set_move(6, 2, State::HUMAN_PIECE);
        s.set_move(7, 2, State::HUMAN_PIECE);

        return s;
}

static inline State tst_state_create_sample4()
{
        State s(8, 8, true, Move(7, 2), 5, 5000);

        s.set_move(1, 5, State::AI_PIECE);
        s.set_move(3, 5, State::AI_PIECE);
        s.set_move(4, 3, State::AI_PIECE);
        s.set_move(2, 1, State::AI_PIECE);

        s.set_move(3, 1, State::AI_PIECE);
        s.set_move(5, 1, State::AI_PIECE);

        s.set_move(2, 4, State::HUMAN_PIECE);
        s.set_move(3, 4, State::HUMAN_PIECE);
        s.set_move(3, 3, State::HUMAN_PIECE);
        s.set_move(3, 2, State::HUMAN_PIECE);

        s.set_move(4, 2, State::HUMAN_PIECE);
        s.set_move(5, 2, State::HUMAN_PIECE);
        s.set_move(4, 1, State::HUMAN_PIECE);

        return s;
}

static inline int tst_state()
{
        State s = ::tst_state_create_sample();
        std::cout << "Preview" << std::endl;
        std::cout << s << std::endl;

        std::cout << "Trying step (3, 3)" << std::endl;
        s.push_move(3, 3, State::AI_PIECE);
        std::cout << s << std::endl;

        std::cout << "Should be goal for AI" << std::endl;
        std::cout << s.is_goal_for(State::AI_PIECE) << std::endl;


        std::cout << "Trying step (5, 0)" << std::endl;
        s.push_move(5, 0, State::HUMAN_PIECE);
        std::cout << s << std::endl;

        std::cout << "Should be goal for human" << std::endl;
        std::cout << s.is_goal_for(State::HUMAN_PIECE) << std::endl;

        std::cout << "Reset all" << std::endl;
        s.reset_all_moves();
        std::cout << s << std::endl;

        std::cout << "Try step (1, 6)" << std::endl;
        s.push_move(1, 6, State::HUMAN_PIECE);
        std::cout << s << std::endl;

        std::cout << "Shouldn't be goal for both" << std::endl;
        std::cout << s.is_goal_for(State::AI_PIECE) << std::endl;
        std::cout << s.is_goal_for(State::HUMAN_PIECE) << std::endl;

        return 0;
}

static inline int tst_state2()
{
        State s = ::tst_state_create_sample4();
        std::cout << "Preview" << std::endl;
        std::cout << s << std::endl;

        std::cout << "Trying step (0, 0)" << std::endl;
        s.push_move(0, 0, State::AI_PIECE);
        std::cout << s << std::endl;

        std::cout << "Trying step (2, 2)" << std::endl;
        s.push_move(2, 2, State::HUMAN_PIECE);
        std::cout << s << std::endl;

        std::cout << "Shouldn't be goal for human" << std::endl;
        std::cout << s.is_goal_for(State::HUMAN_PIECE) << std::endl;
        std::cout << "Shouldn't be goal for AI" << std::endl;
        std::cout << s.is_goal_for(State::AI_PIECE) << std::endl;

        return 0;
}


#endif
