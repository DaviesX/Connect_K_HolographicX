#ifndef TST_STRATEGYDFS_H
#define TST_STRATEGYDFS_H

#include <iostream>
#include <cstdlib>
#include <state.h>
#include <istrategy.h>
#include <strategydfs.h>
#include <tst_state.h>


static int tst_strategydfs()
{

        // State s = ::tst_state_create_sample();
        // State s = ::tst_state_create_sample2();
        // State s = ::tst_state_create_sample4();
        State s = ::tst_state_create_sample5();

        std::cout << "Preview state:" << std::endl;
        std::cout << s << std::endl;

        StrategyDFS strategy;
        Move m;
        strategy.load_state(s);

        std::cout << "Analysis: " << std::endl;
        strategy.print_analysis(std::cout, s, 3);

        std::cout << "Making move: " << std::endl;
        strategy.make_move(s, m);

        std::cout << "AI move: " << std::endl;
        std::cout << m << std::endl;
        return EXIT_SUCCESS;
}

#endif
