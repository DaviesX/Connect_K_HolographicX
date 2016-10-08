#ifndef MOVE_H
#define MOVE_H

class Move
{
public:
	unsigned row; //the row to move to. 
	unsigned col; //the col to move to.
	Move();
	Move(unsigned col, unsigned row);
        
        void set(unsigned x, unsigned y);
};

#endif //MOVE_H
