#include "log.h"
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

void IHeuristic::try_move(const Move& m, int who)
{
}

void IHeuristic::untry_move(const Move& m, int who)
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
