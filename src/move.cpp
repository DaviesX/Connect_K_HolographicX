#include <iostream>
#include "move.h"


Move::Move()
{
        y = 0;
        x = 0;
}

Move::Move(unsigned x, unsigned y)
{
        this->y = y;
        this->x = x;
}

void Move::set(unsigned x, unsigned y)
{
        this->x = x;
        this->y = y;
}

//Move& Move::operator=(const Move& rhs)
//{
//        y = rhs.y;
//        x = rhs.x;
//        return *this;
//}

std::ostream& operator<<(std::ostream& os, const Move& move)
{
        os << "Move = [" << move.x << "," << move.y << "]";
        return os;
}
