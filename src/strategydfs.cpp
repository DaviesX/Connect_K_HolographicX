#include <math.h>
#include <iostream>
#include <log.h>
#include <state.h>
#include <move.h>
#include <strategydfs.h>
#include <actioncostlink.h>


StrategyDFS::StrategyDFS()
{
}

StrategyDFS::~StrategyDFS()
{
}

void StrategyDFS::make_move(const State& s, Move& m)
{
        m.set(0, 0);

        float min = INFINITY;
        for (unsigned y = 0; y < s.num_rows; y ++) {
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        float score = s.g(x, y);
                        std::cout << score << " ";
                        if (s.is(x, y) == State::NO_PIECE) {
                                float score = s.g(x, y);
                                if (score < min) {
                                        m.set(x, y);
                                        min = score;
                                }
                        }
                }
                std::cout << std::endl;
        }

        m.print();
        ::flush();
}

IActionCost* StrategyDFS::get_gxy() const
{
        return new ActionCostLink();
}

IHeuristic* StrategyDFS::get_fxy() const
{
        return nullptr;
}
