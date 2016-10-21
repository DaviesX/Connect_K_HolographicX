#include <math.h>
#include <ostream>
#include <state.h>
#include <move.h>
#include <strategydfs.h>
#include <iheuristic.h>
#include <heursuccesslink.h>


StrategyDFS::StrategyDFS()
{
        m_heur = new HeuristicSuccessLink();
}

StrategyDFS::~StrategyDFS()
{
        delete m_heur;
}

void StrategyDFS::make_move(const State& s, Move& m)
{
        // Min-Max
}

