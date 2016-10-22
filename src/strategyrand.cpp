#include <cstdlib>
#include <move.h>
#include <state.h>
#include <strategyrand.h>


StrategyRandom::StrategyRandom()
{
}

void StrategyRandom::load_state(const State& s)
{
}

void StrategyRandom::make_move(const State& s, Move& m)
{
        int x, y;
        do {
                x = rand()%s.num_cols;
                y = rand()%s.num_rows;
        } while (s.is(x, y) != State::NO_PIECE);

        m.set(x, y);
}
