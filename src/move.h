#ifndef MOVE_H
#define MOVE_H

#include <ostream>

class Move
{
public:
	unsigned row; //the row to move to. 
	unsigned col; //the col to move to.
	Move();
	Move(unsigned col, unsigned row);
        
        void set(unsigned x, unsigned y);
};

std::ostream& operator<<(std::ostream& os, const Move& move);


#endif //MOVE_H
