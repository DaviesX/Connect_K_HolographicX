#include <iostream>
#include <string>
#include <state.h>
#include <move.h>
#include <istrategy.h>
#include <strategyfactory.h>


State obtain_current_state()
{
        static const std::string begin = "makeMoveWithState:";
        static const std::string end = "end";

        while (true) {
                std::string input;
                std::cin >> input;
                if (input == end)
                        exit(EXIT_SUCCESS);
                else if (input == begin){
                        //first I want the gravity, then number of cols, then number of rows,
                        //then the col of the last move, then the row of the last move then the values
                        //for all the spaces.
                        // 0 for no gravity, 1 for gravity
                        //then rows
                        //then cols
                        //then lastMove col
                        //then lastMove row.
                        //then deadline.
                        //add the K variable after deadline.
                        //then the values for the spaces.
                        //cout<<"beginning"<<endl;
                        int g;
                        std::cin >> g;
                        bool gravity = true;
                        if (g == 0)
                                gravity = false;
                        std::cin >> g;
                        int col_count = g;
                        std::cin >> g;
                        int row_count = g;
                        std::cin >> g;
                        int last_move_col = g;
                        std::cin >> g;
                        int last_move_row = g;

                        //add the deadline here:
                        int deadline = -1;
                        std::cin >> g;
                        deadline = g;

                        std::cout << "**deadline**" << deadline << std::endl;

                        std::cin >> g;
                        int k = g;

                        //now the values for each space.
                        //allocate 2D array.
                        State s(col_count, row_count, gravity, 
                                Move(last_move_col, last_move_row), k, deadline);

                        int count_moves = 0;
                        for (int col = 0; col < col_count; col ++) {
                                for (int row = 0; row < row_count; row ++) {
                                        int who;
                                        std::cin >> who;
                                        s.set_move(col, row, who);
                                        if (who != State::NO_PIECE)
                                                count_moves += who;
                                }
                        }
                        return s;
                } else
                        //otherwise loop back to the top and wait for proper input.
                        std::cout << "unrecognized command " << input << std::endl;
        }
}

void return_move(const Move& move)
{
        std::string made_move = "ReturningTheMoveMade";
        //outputs made_move then a space then the row then a space then the column
        //then a line break.
        std::cout << made_move << " " << move.col << " " << move.row << std::endl;
}

// Unit tests
#include <tst_state.h>
#include <tst_heursuccesslink.h>
#include <tst_strategydfs.h>

int main() 
{
#if 0
        std::cout << "Make sure this program is ran by the Java shell. It is incomplete on its own. " << std::endl;
        IStrategy* strategy = StrategyFactory().create(StrategyFactory::DFS);
        do { 
                const State& state = ::obtain_current_state();
                Move m;
                strategy->make_move(state, m);
                ::return_move(m);
        } while (true);
        delete strategy;
        return EXIT_SUCCESS;
#else
        // return tst_heursuccesslink();
        // return tst_strategydfs();
        return tst_state();
#endif
}
