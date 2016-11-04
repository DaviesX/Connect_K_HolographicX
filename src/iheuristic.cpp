#include "log.h"
#include "state.h"
#include "iheuristic.h"


IHeuristic::IHeuristic()
{
}

IHeuristic::~IHeuristic()
{
}

void IHeuristic::load_state(const State& s)
{
}

void IHeuristic::try_move(const State& s, const Move& m)
{
}

void IHeuristic::untry_move()
{
}

void IHeuristic::print(std::ostream& os) const
{
        os << "IHeuristic = [none]";
}

std::ostream& operator<<(std::ostream& os, const IHeuristic& h)
{
        h.print(os);
        return os;
}
