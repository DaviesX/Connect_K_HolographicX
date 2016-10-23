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
        void            print_analysis(std::ostream& os, const State& s, int depth);
private:
        float           minimizer(State& s, const Move& move, unsigned depth, const unsigned& limit);
        float           maximizer(State& s, const Move& move, unsigned depth, const unsigned& limit);
        float           min_max_move(State& s, unsigned limit, Move& move);
        IHeuristic*     m_heur;
};


#endif  // STRATEGYDFS
