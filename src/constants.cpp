#include "constants.h"

float constants::b;
float constants::c;

void constants::init()
{
        b = 0.01f;
        c = 2.0f;
}

void constants::load_monte_carlo(std::istream& s)
{
        s >> b;
        s >> c;
}

void constants::save_monte_carlo(std::ostream& s)
{
        s << b;
        s << c;
}
