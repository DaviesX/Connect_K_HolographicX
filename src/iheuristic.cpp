#include <log.h>
#include <iheuristic.h>


IHeuristic::IHeuristic()
{
}

IHeuristic::~IHeuristic()
{
}

void IHeuristic::load_state(const State& s)
{
}

void IHeuristic::accept(const Move& m, int who, float score)
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
