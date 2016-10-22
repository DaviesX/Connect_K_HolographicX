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

        State s = ::tst_state_create_sample();

        StrategyDFS strategy;
        Move m;
        strategy.load_state(s);
        strategy.make_move(s, m);

        std::cout << "AI move: " << std::endl;
        std::cout << m << std::endl;
        return EXIT_SUCCESS;
}

#endif
