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
        struct AvailableAction
        {
                AvailableAction(int x, int y, int score):
                        x(x), y(y), score(score)
                {
                }

                bool operator < (const AvailableAction& action) const
                {
                        return score < action.score;
                }

                bool operator > (const AvailableAction& action) const
                {
                        return score > action.score;
                }

                int     x;
                int     y;
                float   score;
        };

        float           minimizer(State& s, const Move& move, unsigned depth, const unsigned& limit);
        float           maximizer(State& s, const Move& move, unsigned depth, const unsigned& limit);
        float           min_max_move(State& s, unsigned limit, Move& move);
        void            build_actions(State& s, std::vector<AvailableAction>& actions);
        IHeuristic*     m_heur;
};


#endif  // STRATEGYDFS
