#ifndef MOVE_H
#define MOVE_H

#include <ostream>

class Move
{
public:
        unsigned y; //the row to move to.
        unsigned x; //the col to move to.
        Move();
        Move(unsigned x, unsigned y);

        void set(unsigned x, unsigned y);

        //Move& operator=(const Move& rhs);
};

std::ostream& operator<<(std::ostream& os, const Move& move);


#endif //MOVE_H
