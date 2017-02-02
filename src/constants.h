#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <istream>
#include <ostream>

namespace constants
{
        extern float b;
        extern float c;

        void init();
        void load_monte_carlo(std::istream& s);
        void save_monte_carlo(std::ostream& s);
};

#endif // CONSTANTS_H
