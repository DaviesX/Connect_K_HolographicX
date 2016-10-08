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
                        int colCount = g;
                        std::cin >> g;
                        int rowCount = g;
                        std::cin >> g;
                        int lastMoveCol = g;
                        std::cin >> g;
                        int lastMoveRow = g;

                        //add the deadline here:
                        int deadline = -1;
                        std::cin >> g;
                        deadline = g;

                        std::cin >> g;
                        int k = g;

                        //now the values for each space.
                        //allocate 2D array.
                        int** gameState = NULL;
                        gameState = new int*[colCount];
                        for (int i = 0; i < colCount; i ++)
                                gameState[i] = new int[rowCount];

                        int countMoves = 0;
                        for (int col = 0; col < colCount; col ++) {
                                for (int row = 0; row<rowCount; row ++) {
                                        std::cin >> gameState[col][row];
                                        if (gameState[col][row] != State::NO_PIECE) {
                                                countMoves += gameState[col][row];
                                        }
                                }
                        }

                        return State(colCount, rowCount, gravity, gameState, Move(lastMoveCol, lastMoveRow), k, deadline);
                } else
                        //otherwise loop back to the top and wait for proper input.
                        std::cout << "unrecognized command " << input << std::endl;
        }
}

void return_move(const Move& move)
{
        std::string madeMove = "ReturningTheMoveMade";
        //outputs madeMove then a space then the row then a space then the column
        //then a line break.
        std::cout << madeMove << " " << move.col << " " << move.row << std::endl;
}

int main() 
{
        std::cout << "Make sure this program is ran by the Java shell. It is incomplete on its own. " << std::endl;
        IStrategy* strategy = StrategyFactory().create(StrategyFactory::Random);
        do { 
                const State& state = ::obtain_current_state();
                Move m;
                strategy->make_move(state, m);
                ::return_move(m);
        } while (true);
        delete strategy;
        return EXIT_SUCCESS;
}
