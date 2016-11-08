#ifndef TST_STRATEGYDFS_H
#define TST_STRATEGYDFS_H

#include <iostream>
#include <cstdlib>
#include "state.h"
#include "istrategy.h"
#include "strategydfs.h"
#include "tst_state.h"

static bool is_expected(const StrategyDFS& strategy, const State& s, unsigned d, unsigned t,
                        const std::vector<Move>& moves, Move& made)
{
        strategy.make_move(s, d, t, made);
        for (Move expected: moves) {
                if (made == expected)
                        return true;
        }
        return false;
}

static int tst_strategydfs()
{
        std::vector<State> states;
        std::vector<std::vector<Move>> expected;

        states.push_back(::tst_state_create_sample());
        std::vector<Move> m1;
        m1.push_back(Move(3, 3));
        expected.push_back(m1);

        states.push_back(::tst_state_create_sample2());
        std::vector<Move> m2;
        m2.push_back(Move(3, 3));
        m2.push_back(Move(4, 3));
        m2.push_back(Move(5, 3));
        expected.push_back(m2);

        states.push_back(::tst_state_create_sample3());
        std::vector<Move> m3;
        m3.push_back(Move(7, 0));
        expected.push_back(m3);

        states.push_back(::tst_state_create_sample4());
        std::vector<Move> m4;
        m4.push_back(Move(2, 2));
        m4.push_back(Move(6, 2));
        expected.push_back(m4);

        states.push_back(::tst_state_create_sample5());
        std::vector<Move> m5;
        m5.push_back(Move(0, 2));
        expected.push_back(m5);

        states.push_back(::tst_state_create_sample6());
        std::vector<Move> m6;
        m6.push_back(Move(2, 3));
        expected.push_back(m6);

        states.push_back(::tst_state_create_sample7());
        std::vector<Move> m7;
        m7.push_back(Move(5, 2));
        expected.push_back(m7);

        states.push_back(::tst_state_create_sample8());
        std::vector<Move> m8;
        m8.push_back(Move(1, 0));
        m8.push_back(Move(5, 4));
        expected.push_back(m8);

        states.push_back(::tst_state_create_sample9());
        std::vector<Move> m9;
        m9.push_back(Move(3, 6));
        expected.push_back(m9);

        states.push_back(::tst_state_create_sample10());
        std::vector<Move> m10;
        m10.push_back(Move(4, 1));
        expected.push_back(m10);

        states.push_back(::tst_state_create_sample11());
        std::vector<Move> m11;
        m11.push_back(Move(0, 0));
        expected.push_back(m11);

        const unsigned t = 5000;
        const unsigned d = 3;
        const bool escape = true;

        StrategyDFS strategy;

        //strategy.print_analysis(std::cout, states[6], 3, 5, 2);
#if 1
        Move m;
        for (unsigned i = 11; i < states.size(); i ++) {
                std::cout << "Preview state: " << i + 1 << std::endl;
                std::cout << states[i] << std::endl;

                if (is_expected(strategy, states[i], d, t, expected[i], m)) {
                        std::cout << "Passed sample " << i + 1 << std::endl;
                } else {
                        std::cout << "Failing sample " << i + 1 << std::endl;
                        std::cout << "Analysis: " << std::endl;
                        strategy.print_analysis(std::cout, states[i], d);
                        std::cout << "AI move: " << std::endl;
                        std::cout << m << std::endl;
                        std::cout << "But expecting: ";
                        for (Move m: expected[i]) {
                                std::cout << m << "\t";
                        }
                        std::cout << std::endl;
                        std::cout << "Analysis of the expected move: ";
                        for (Move m: expected[i]) {
                                std::cout << m << std::endl;
                                strategy.print_analysis(std::cout, states[i], d, m.x, m.y);
                        }
                        std::cout << std::endl;
                }

                if (escape)
                        break;
        }
#endif
        return EXIT_SUCCESS;
}

#endif
