#ifndef STATE_H
#define STATE_H

#include <ostream>
#include <vector>
#include <algorithm>
#include "move.h"

typedef bool (*scan_eval_t) (const char* val, int x, int y, unsigned dist, void* data);
typedef char player_t;

// Helpers
static unsigned inline move_xy(const char* board, const int w, const int h, const int k,
                               int& x, int& y, const unsigned d,
                               scan_eval_t eval, void* data)
{
        int counter = 0;

        // Multiplex each scenarios,
        // so we save the number of comparisons in the long run.
        switch (d) {
                case 0:         // 0 degree.
                        counter = x;
                        do {
                                x ++;
                        } while (x < w && x - counter < k &&
                                 eval(&board[x + w*y], x, y, x - counter, data));
                        counter = x - counter;
                        break;
                case 1:         // 45 degree.
                        counter = x;
                        do {
                                x ++;
                                y --;
                        } while (x < w && y >= 0 && x - counter < k &&
                                 eval(&board[x + w*y], x, y, x - counter, data));
                        counter = x - counter;
                        break;
                case 2:         // 90 degree.
                        counter = y;
                        do {
                                y --;
                        } while (y >= 0 && counter - y < k &&
                                 eval(&board[x + w*y], x, y, counter - y, data));
                        counter = counter - y;
                        break;
                case 3:         // 135 degree.
                        counter = y;
                        do {
                                x --;
                                y --;
                        } while (x >= 0 && y >= 0 && counter - y < k &&
                                 eval(&board[x + w*y], x, y, counter - y, data));
                        counter = counter - y;
                        break;
                case 4:         // 180 degree.
                        counter = x;
                        do {
                                x --;
                        } while (x >= 0 && counter - x < k &&
                                 eval(&board[x + w*y], x, y, counter - x, data));
                        counter = counter - x;
                        break;
                case 5:         // 225 degree.
                        counter = x;
                        do {
                                x --;
                                y ++;
                        } while (x >= 0 && y < h && counter - x < k &&
                                 eval(&board[x + w*y], x, y, counter - x, data));
                        counter = counter - x;
                        break;
                case 6:         // 270 degree.
                        counter = y;
                        do {
                                y ++;
                        } while (y < h && y - counter < k &&
                                 eval(&board[x + w*y], x, y, y - counter, data));
                        counter = y - counter;
                        break;
                case 7:         // 315 degree.
                        counter = y;
                        do {
                                x ++;
                                y ++;
                        } while (x < w && y < h && y - counter < k &&
                                 eval(&board[x + w*y], x, y, y - counter, data));
                        counter = y - counter;
                        break;
        }
        return counter;
}

static unsigned inline scan_on(const char* board, const int w, const int h, const int k,
                               int x, int y, const unsigned d,
                               scan_eval_t eval, void* data)
{
        return ::move_xy(board, w, h, k, x, y, d, eval, data);
}


/*
 * <State> represents currect game state.
 */
class State
{
        friend std::ostream& operator<<(std::ostream& os, const State& s);
public:
        //these represent the values for each piece type.
        static const char AI_PIECE               = 1;
        static const char HUMAN_PIECE            = -1;
        static const char NO_PIECE               = 0;

public:
        const unsigned  num_rows;       // The total number of rows in the game state.
        const unsigned  num_cols;       // The total number of columns in the game state.
        unsigned        num_left;

        const bool      gravity_on;     // This will be true if gravity is turned on. It will be false if gravity is turned off.
        const Move      last_move;      // This is the move made last by your opponent. If your opponent has not made a move yet (you move first) then this move will hold the value (-1, -1) instead.

        const unsigned  deadline;       // This is how many milliseconds the AI has to make move.
        const unsigned  k;              // k is the number of pieces a player must get in a row/column/diagonal to win the game. IE in connect 4, this variable would be 4

        State(const unsigned num_cols,
              const unsigned num_rows,
              const bool gravity_on,
              const Move& last_move,
              const unsigned k,
              const unsigned deadline);
        State(const State& s);
        ~State();

        struct MiniNode
        {
                unsigned x;
                unsigned y;

                MiniNode(unsigned x, unsigned y):
                        x(x), y(y)
                {
                }

                MiniNode(const MiniNode& node):
                        x(node.x), y(node.y)
                {
                }

                void print(std::ostream& os) const
                {
                        os << "MiniNode = [" << x << "," << y << "]";
                }
        };

        bool            operator== (const State& other) const;

        player_t        is(unsigned x, unsigned y) const;
        void            set_move(unsigned x, unsigned y, player_t who);

        bool            is_goal() const;
        bool            is_goal_for(const Move& m, player_t who) const;
        bool            is_almost_goal_for(const Move& m, player_t who) const;
        bool            is_steady_goal_for(const Move& m, player_t who) const;
        bool            is_goal_for(player_t who) const;

        unsigned        scan(int x, int y, unsigned d, scan_eval_t eval, void* data) const;
        unsigned        move_xy(int& x, int& y, unsigned d, scan_eval_t eval, void* data) const;

        bool            collides_edges(int x, int y, unsigned d, int dist) const;
        bool            collides_edges(int x, int y, unsigned d) const;

        float           current_score() const;
        const std::vector<State::MiniNode>& path() const;


        void            push_move(unsigned x, unsigned y, char who);
        void            prev_move(Move& move) const;
        void            pop_move();
        void            reset_all_moves();

        template<typename T>
        void            expand(std::vector<T>& expansion) const;
private:
        int                             m_goal_for = State::NO_PIECE;   // Whose goal?
        player_t*                       m_board;                        // Gameboard
        std::vector<State::MiniNode>    m_stack;                        // Action path
};

std::ostream&   operator<<(std::ostream& os, const State& s);

inline unsigned State::scan(int x, int y, unsigned d, scan_eval_t eval, void* data) const
{
        return ::scan_on(m_board, static_cast<int>(num_cols), static_cast<int>(num_rows),
                         static_cast<int>(k), x, y, d, eval, data);
}

inline unsigned State::move_xy(int& x, int& y, unsigned d, scan_eval_t eval, void* data) const
{
        return ::move_xy(m_board, static_cast<int>(num_cols), static_cast<int>(num_rows),
                         static_cast<int>(k), x, y, d, eval, data);
}

template <typename T>
void State::expand(std::vector<T>& result) const
{
        result.clear();

        if (!gravity_on) {
                for (unsigned y = 0; y < num_rows; y ++) {
                        for (unsigned x = 0; x < num_cols; x ++) {
                                if (is(x, y) != State::NO_PIECE)
                                        continue;
                                result.push_back(T(Move(x, y)));
                        }
                }
        } else {
                for (unsigned x = 0; x < num_cols; x ++) {
                        for (unsigned y = 0; y < num_rows; y ++) {
                                if (is(x, y) == State::NO_PIECE) {
                                        result.push_back(T(Move(x, y)));
                                        break;
                                }
                        }
                }
                std::sort(result.begin(), result.end(), [](const T& a, const T& b) {
                        return static_cast<const Move&>(a) < static_cast<const Move&>(b);
                });
        }
}

#define opponent_of(__who)      ((__who) == State::AI_PIECE ? State::HUMAN_PIECE : State::AI_PIECE)


#endif // STATE_H
