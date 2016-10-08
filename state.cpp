#include <state.h>


State::State(const unsigned num_cols, 
             const unsigned num_rows, 
             const bool gravity_on, 
             int** const game_state, 
             const Move& last_move,
             const unsigned k,
             const unsigned deadline):
        num_rows(num_rows), 
        num_cols(num_cols), 
        gravity_on(gravity_on),
        game_state(game_state),
        last_move(last_move),
        k(k),
        deadline(deadline)
{
}


State::~State()
{
	
	//delete the gameState variable.
	for (unsigned i = 0; i < num_cols; i ++) {
		delete [] game_state[i];
	}
	delete [] game_state;

}
