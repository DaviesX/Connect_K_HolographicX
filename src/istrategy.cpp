#include "istrategy.h"


IStrategy::IStrategy()
{
}

IStrategy::~IStrategy()
{
}

void IStrategy::load_state(const State& s)
{
}

void IStrategy::print(std::ostream& os) const
{
        os << "IStrategy = [Not implemented]";
}


std::ostream& operator<<(std::ostream& os, const IStrategy& s)
{
        s.print(os);
        return os;
}
