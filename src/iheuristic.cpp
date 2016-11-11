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

bool IHeuristic::is_goal_for(const State& s, const Move& next_move, int who) const
{
        return s.is_goal_for(next_move, who);
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
