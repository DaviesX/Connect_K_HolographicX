#ifndef STRATEGYDFS
#define STRATEGYDFS

#include <iheuristic.h>
#include <istrategy.h>


class StrategyDFS: public IStrategy
{
public:
        StrategyDFS();
        ~StrategyDFS() override;

        void            make_move(const State& s, Move& m) override;
private:
        IHeuristic*     m_heur;
        bool            m_first_time;
};


#endif  // STRATEGYDFS
