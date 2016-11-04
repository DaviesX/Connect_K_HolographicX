#include <iostream>
#include "move.h"


Move::Move()
{
    row = 0;
    col = 0;
}

Move::Move(unsigned col, unsigned row)
{
    this->row = row;
    this->col = col;
}

void Move::set(unsigned x, unsigned y)
{
        col = x;
        row = y;
}

std::ostream& operator<<(std::ostream& os, const Move& move)
{
        os << "Move = [" << move.col << "," << move.row << "]";
        return os;
}
