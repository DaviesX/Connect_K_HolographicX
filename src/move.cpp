#include <iostream>
#include "move.h"


Move::Move()
{
        y = 0;
        x = 0;
}

Move::Move(unsigned char x, unsigned char y)
{
        this->y = y;
        this->x = x;
}

void Move::set(unsigned char x, unsigned char y)
{
        this->x = x;
        this->y = y;
}

Move& Move::operator=(const Move& rhs)
{
        y = rhs.y;
        x = rhs.x;
        return *this;
}

bool Move::operator== (const Move& rhs) const
{
        return key == rhs.key;
}

bool Move::operator < (const Move& rhs) const
{
        return key < rhs.key;
}

std::ostream& operator<<(std::ostream& os, const Move& move)
{
        os << "Move = [" << (int) move.x << "," << (int) move.y << "]";
        return os;
}
