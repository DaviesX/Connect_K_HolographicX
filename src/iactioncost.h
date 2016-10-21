#ifndef IACTIONCOST_H
#define IACTIONCOST_H

#include <ostream>

class State;
class Move;

class IActionCost
{
        friend std::ostream& operator<<(std::ostream& os, const IActionCost& ac);
public:
        IActionCost();
        virtual ~IActionCost();
        virtual void    load_state(const State& s) = 0;
        virtual void    accept(const State& s, float score, unsigned x, unsigned y) = 0;
        virtual float   evaluate(const State& s, unsigned x, unsigned y) = 0;
        virtual void    print(std::ostream& os) const;
};

std::ostream& operator<<(std::ostream& os, const IActionCost& ac);


#endif  // IACTIONCOST_H
