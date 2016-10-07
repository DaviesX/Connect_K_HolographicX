#include "aishell.h"
#include <stdlib.h>
#include <iostream>


AIShell::AIShell(const unsigned num_cols, const unsigned num_rows, const bool gravity_on, int** const game_state, const Move& last_move):
        m_num_rows(num_rows), 
        m_num_cols(num_cols), 
        m_gravity_on(gravity_on),
        m_game_state(game_state),
        m_last_move(last_move),
        deadline(0)
{
}


AIShell::~AIShell()
{
	
	//delete the gameState variable.
	for (unsigned i = 0; i < m_num_cols; i ++) {
		delete [] m_game_state[i];
	}
	delete [] m_game_state;

}

Move AIShell::make_move() const
{
	// @TODO: Implementation goes here.
        int x, y;
        do {
                x = rand()%m_num_cols;
                y = rand()%m_num_rows;
        } while (m_game_state[x][y] != NO_PIECE);
	return Move(x, y);
}
