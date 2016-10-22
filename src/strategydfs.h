#ifndef STRATEGYDFS
#define STRATEGYDFS

#include <iheuristic.h>
#include <istrategy.h>


class StrategyDFS: public IStrategy
{
public:
        StrategyDFS();
        ~StrategyDFS() override;

        void            load_state(const State& s) override;
        void            make_move(const State& s, Move& m) override;
private:
        IHeuristic*     m_heur;
};


#endif  // STRATEGYDFS
