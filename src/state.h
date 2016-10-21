#ifndef STATE_H 
#define STATE_H 

#include <move.h>
#include <ostream>
#include <vector>


/*
 * <State> represents currect game state.
 */
class State 
{
        friend std::ostream& operator<<(std::ostream& os, const State& s);
public:
	//these represent the values for each piece type.
	static const int AI_PIECE               = 1;
	static const int HUMAN_PIECE            = -1;
	static const int NO_PIECE               = 0;

public:
	const unsigned  num_rows;       // The total number of rows in the game state.
	const unsigned  num_cols;       // The total number of columns in the game state.

	const bool      gravity_on;     // This will be true if gravity is turned on. It will be false if gravity is turned off.
	const Move      last_move;      // This is the move made last by your opponent. If your opponent has not made a move yet (you move first) then this move will hold the value (-1, -1) instead.

	const unsigned  deadline;       // This is how many milliseconds the AI has to make move.
	const unsigned  k;              // k is the number of pieces a player must get in a row/column/diagonal to win the game. IE in connect 4, this variable would be 4

        State(const unsigned num_cols, 
              const unsigned num_rows, 
              const bool gravity_on, 
              const Move& last_move,
              const unsigned k,
              const unsigned deadline);
	~State();

        struct MiniNode
        {
                unsigned x;
                unsigned y;
                float score;

                MiniNode(unsigned x, unsigned y, float score):
                        x(x), y(y), score(score)
                {
                }

                MiniNode(const MiniNode& node):
                        x(node.x), y(node.y), score(node.score)
                {
                }

                void print(std::ostream& os) const
                {
                        os << "MiniNode = [" << x << "," << y << "," << score << "]";
                }
        };

        const int                               is(unsigned x, unsigned y) const;
        void                                    set_move(unsigned x, unsigned y, int who);
        bool                                    is_goal_for(int who) const;
        float                                   current_score() const;
        const std::vector<State::MiniNode>&     path() const;


        void            push_move(unsigned x, unsigned y, int who, float score);
        void            pop_move();
        void            reset_all_moves();
private:
        float                           m_cur_score = 0;                // Current path cost of the state.
        int                             m_goal_for = State::NO_PIECE;   // Whose goal?
	int*                            m_board;                        // Gameboard
        std::vector<State::MiniNode>    m_stack;                        // Action path
};

std::ostream& operator<<(std::ostream& os, const State& s);


#endif // STATE_H
