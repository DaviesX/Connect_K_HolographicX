#ifndef MOVE_H
#define MOVE_H

#include <ostream>

class Move
{
public:
        union {
                unsigned char y; //the row to move to.
                unsigned char x; //the col to move to.
                unsigned short key;
        };

        Move();
        Move(unsigned char x, unsigned char y);

        void set(unsigned char x, unsigned char y);

        Move& operator= (const Move& rhs);
        bool operator== (const Move& rhs);
};

std::ostream& operator<<(std::ostream& os, const Move& move);


#endif //MOVE_H
