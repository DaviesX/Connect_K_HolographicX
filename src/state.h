#ifndef STATE_H 
#define STATE_H 

#include <move.h>
#include <vector>

class IActionCost;
class IHeuristic;

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
	const unsigned  num_rows;       // The total number of rows in the game state.
	const unsigned  num_cols;       // The total number of columns in the game state.
	int** const     game_state;     // A pointer to a two-dimensional array representing the game state.
	const bool      gravity_on;     // This will be true if gravity is turned on. It will be false if gravity is turned off.
	const Move      last_move;      // This is the move made last by your opponent. If your opponent has not made a move yet (you move first) then this move will hold the value (-1, -1) instead.

	unsigned        deadline;       // This is how many milliseconds the AI has to make move.
	unsigned        k;              // k is the number of pieces a player must get in a row/column/diagonal to win the game. IE in connect 4, this variable would be 4

        float           cost;           // Current path cost of the state.

        State(const unsigned num_cols, 
              const unsigned num_rows, 
              const bool gravity_on, 
              int** const game_state, 
              const Move& last_move,
              const unsigned k,
              const unsigned deadline,
              const IActionCost* fcost,
              const IHeuristic* heuristic);
	~State();

        struct MiniNode
        {
                unsigned x;
                unsigned y;
                float cost;

                MiniNode(unsigned x, unsigned y, float cost):
                        x(x), y(y), cost(cost)
                {
                }

                MiniNode(const MiniNode& node):
                        x(node.x), y(node.y), cost(node.cost)
                {
                }
        };

        const int                               is(unsigned x, unsigned y) const;
        float                                   g(unsigned x, unsigned y) const;
        float                                   f(unsigned x, unsigned y) const;
        bool                                    is_goal() const;
        const std::vector<State::MiniNode>&     path() const;

        void            push_move(unsigned x, unsigned y, int who);
        void            pop_move();
        void            reset();
private:
        std::vector<State::MiniNode>    m_stack;
        const IActionCost*              m_fcost;
        const IHeuristic*               m_heuristic;
};

#endif // STATE_H
