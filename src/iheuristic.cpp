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

float IHeuristic::coarse_eval(const State& s, const Move& next_move) const
{
        return 0;
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
