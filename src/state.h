#ifndef STATE_H 
#define STATE_H 

#include <move.h>

/*
 * <State> represents currect game state.
 */
class State 
{
public:
	//these represent the values for each piece type.
	static const int AI_PIECE               = 1;
	static const int HUMAN_PIECE            = -1;
	static const int NO_PIECE               = 0;


public:
	const unsigned  num_rows;       //the total number of rows in the game state.
	const unsigned  num_cols;       //the total number of columns in the game state.
	int** const     game_state;     //a pointer to a two-dimensional array representing the game state.
	const bool      gravity_on;     //this will be true if gravity is turned on. It will be false if gravity is turned off.
	const Move      last_move;      //this is the move made last by your opponent. If your opponent has not made a move yet (you move first) then this move will hold the value (-1, -1) instead.

	int             deadline;       //this is how many milliseconds the AI has to make move.
	int             k;              // k is the number of pieces a player must get in a row/column/diagonal to win the game. IE in connect 4, this variable would be 4

        State(const unsigned num_cols, 
              const unsigned num_rows, 
              const bool gravity_on, 
              int** const game_state, 
              const Move& last_move,
              const unsigned k,
              const unsigned deadline);
	~State();
};

#endif // STATE_H
