#ifndef HEURULTIMATE_H
#define HEURULTIMATE_H

#include <vector>
#include "move.h"
#include "state.h"
#include "iheuristic.h"


class HeuristicUltimate: public IHeuristic
{
public:
        HeuristicUltimate();
        ~HeuristicUltimate();

        void    try_move(const State& s, const Move& m) override;
        void    untry_move() override;
        void    load_state(const State& s) override;
        float   coarse_eval(const State& s, const Move& next_move) const override;
        float   evaluate(const State& s, const Move& next_move) const override;
        void    print(std::ostream& os) const override;

        // Link statistics.
        struct LinkStat
        {
                LinkStat(int who):
                        who(who)
                {
                        oppo = opponent_of(who);
                }

                LinkStat():
                        LinkStat(State::AI_PIECE)
                {
                }

                void reset()
                {
                        ins = 0;
                        del = 0;
                        state = 0;
                }

                int             who;
                unsigned        ins = 0;
                unsigned        state = 0;
                unsigned        del = 0;
                int             oppo;
        };

        const static short REF_NULL = -1;
        const static short REF_DEAD_MOVE = -2;

        struct ConnectedComponent {
                unsigned short  parent;
                unsigned char   ins;
                unsigned short  del;

                ConnectedComponent()
                {
                        parent = REF_NULL;
                        ins = 0;
                        del = 0;
                }
        };

        struct ComponentReference {
                struct Stat {
                        short           ref = REF_NULL;
                        unsigned char   del = 0;
                        bool            is_peripheral = false;
                } stat[2][4];

                ComponentReference()
                {
                }

                short* parent_m(int who, unsigned dir)
                {
                        if (who == State::AI_PIECE)
                                return &stat[0][dir].ref;
                        else
                                return &stat[1][dir].ref;
                }

                short parent(int who, unsigned dir) const
                {
                        if (who == State::AI_PIECE)
                                return stat[0][dir].ref;
                        else
                                return stat[1][dir].ref;
                }

                unsigned char* deletion_m(int who, unsigned dir)
                {
                        if (who == State::AI_PIECE)
                                return &stat[0][dir].del;
                        else
                                return &stat[1][dir].del;
                }

                unsigned char deletion(int who, unsigned dir) const
                {
                        if (who == State::AI_PIECE)
                                return stat[0][dir].del;
                        else
                                return stat[1][dir].del;
                }

                bool* is_peripheral_m(int who, unsigned dir)
                {
                        if (who == State::AI_PIECE)
                                return &stat[0][dir].is_peripheral;
                        else
                                return &stat[1][dir].is_peripheral;
                }

                bool is_peripheral(int who, unsigned dir) const
                {
                        if (who == State::AI_PIECE)
                                return stat[0][dir].is_peripheral;
                        else
                                return stat[1][dir].is_peripheral;
                }
        };

private:
        float   evaluate_move(const State& s, const Move& move, int who) const;
        float   benefit(const State& s, const Move& next_move, int who, int extra_moves) const;

        void    mark_dead_moves(const State& s, int who);
        void    extract_connected_components(const State& s, int who);

        const ConnectedComponent&       allocate_cnn(unsigned short& ref);
        const ConnectedComponent&       obtain_cnn(unsigned short ref) const;
        const ConnectedComponent&       obtain_root_cnn(unsigned short ref) const;
        void                            clear_cnn();
        const ComponentReference&       cnn_map(unsigned char x, unsigned char y) const;
        ComponentReference*             cnn_map_m(unsigned char x, unsigned char y);

        ConnectedComponent*     m_cnn_buf = nullptr;
        unsigned short          m_num_cnn = 0;
        ComponentReference*     m_cnn_map = nullptr;
        unsigned                m_cnn_w = 0;
        unsigned                m_cnn_h = 0;

        std::vector<Move>       m_stack;
};

#endif // HEURULTIMATE_H
