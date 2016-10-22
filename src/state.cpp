#include <ostream>
#include <iostream>
#include <log.h>
#include <iheuristic.h>
#include <state.h>


// Helpers
static int inline scan_on(const int* board, const unsigned w, const unsigned h, const int k, 
                          int x, int y, const unsigned d, 
                          scan_eval_t eval, void* data)
{
        int counter;

        // Multiplex each scenarios, 
        // so we save the number of comparisons in the long run.
        switch (d) {
                case 0:         // 0 degree.
                        counter = x;
                        while (x < w && x - counter < k && 
                               eval(&board[x + w*y], x - counter, data))
                                x ++;
                        counter = x - counter;
                        break;
                case 1:         // 45 degree.
                        counter = x;
                        while (x < w && y >= 0 && x - counter < k && 
                               eval(&board[x + w*y], x - counter, data)) {
                                x ++;
                                y --; 
                        }
                        counter = x - counter;
                        break;
                case 2:         // 90 degree.
                        counter = y;
                        while (y >= 0 && counter - y < k && 
                               eval(&board[x + w*y], counter - y, data))
                                y --; 
                        counter = counter - y;
                        break;
                case 3:         // 135 degree.
                        counter = y;
                        while (x >= 0 && y >= 0 && counter - y < k && 
                               eval(&board[x + w*y], counter - y, data)) {
                                x --; 
                                y --; 
                        }
                        counter = counter - y;
                        break;
                case 4:         // 180 degree.
                        counter = x;
                        while (x >= 0 && counter - x < k &&
                               eval(&board[x + w*y], counter - x, data))
                                x --;
                        counter = counter - x;
                        break;
                case 5:         // 225 degree.
                        counter = x;
                        while (x >= 0 && y < h && counter - x < k &&
                               eval(&board[x + w*y], counter - x, data)) {
                                x --; 
                                y ++; 
                        }
                        counter = counter - x;
                        break;
                case 6:         // 270 degree.
                        counter = y;
                        while (y < h && y - counter < k &&
                               eval(&board[x + w*y], y - counter, data))
                                y ++;
                        counter = y - counter;
                        break;
                case 7:         // 315 degree.
                        counter = y;
                        while (x < w && y < h && y - counter < k &&
                               eval(&board[x + w*y], y - counter, data)) {
                                x ++;
                                y ++;
                        }
                        counter = y - counter;
                        break;
        }
        return counter;
}

static bool goal_eval(const int* val, unsigned dist, void* data)
{
        return *val == *(int*) data;
}

static bool is_goal_for(const int* board, unsigned w, unsigned h, const Move& move, int who, unsigned k)
{
        return ::scan_on(board, w, h, k, move.col, move.row, 0, ::goal_eval, &who) >= k || 
               ::scan_on(board, w, h, k, move.col, move.row, 1, ::goal_eval, &who) >= k || 
               ::scan_on(board, w, h, k, move.col, move.row, 2, ::goal_eval, &who) >= k || 
               ::scan_on(board, w, h, k, move.col, move.row, 3, ::goal_eval, &who) >= k || 
               ::scan_on(board, w, h, k, move.col, move.row, 4, ::goal_eval, &who) >= k || 
               ::scan_on(board, w, h, k, move.col, move.row, 5, ::goal_eval, &who) >= k || 
               ::scan_on(board, w, h, k, move.col, move.row, 6, ::goal_eval, &who) >= k || 
               ::scan_on(board, w, h, k, move.col, move.row, 7, ::goal_eval, &who) >= k;
}

// APIs
State::State(const unsigned num_cols, 
             const unsigned num_rows, 
             const bool gravity_on, 
             const Move& last_move,
             const unsigned k,
             const unsigned deadline):
        num_rows(num_rows), 
        num_cols(num_cols), 
        gravity_on(gravity_on),
        last_move(last_move),
        k(k),
        deadline(deadline)
{
        m_board = new int [num_cols*num_rows];
}


State::~State()
{
	delete [] m_board;
}

const int State::is(unsigned x, unsigned y) const
{
        return m_board[x + y*num_cols];
}

void State::set_move(unsigned x, unsigned y, int who)
{
        m_board[x + y*num_cols] = who;
}

bool State::is_goal_for(int who) const
{
        return m_goal_for == who;
}

void State::scan(int x, int y, unsigned d, scan_eval_t eval, void* data) const
{
        ::scan_on(m_board, num_cols, num_rows, k, x, y, d, eval, data);
}

const std::vector<State::MiniNode>& State::path() const
{
        return m_stack;
}

void State::push_move(unsigned x, unsigned y, int who, float score)
{
        if (::is_goal_for(m_board, num_cols, num_rows, Move(x, y), who, k))
                m_goal_for = who;
        m_stack.push_back(State::MiniNode(x, y, m_cur_score));
        m_board[x + y*num_cols] = who;
        m_cur_score = score; 
}

void State::pop_move()
{
        const State::MiniNode& node = m_stack.back();
        m_board[node.x + node.y*num_cols] = State::NO_PIECE;
        m_goal_for = State::NO_PIECE;
        m_cur_score = node.score;
        m_stack.pop_back();
}

void State::reset_all_moves()
{
        while (!m_stack.empty())
                pop_move();
}

std::ostream& operator<<(std::ostream& os, const State& s)
{
        os << "State = [";
        // Gameboard:
        os << "board:" << std::endl;
        for (unsigned y = 0; y < s.num_rows; y ++) {
                os << "\t";
                for (unsigned x = 0; x < s.num_cols; x ++) {
                        os << s.m_board[x + y*s.num_cols] << " ";
                }
                os << std::endl;
        }
        // Path:
        os << ", path:\t" << std::endl;
        for (unsigned i = 0; i < s.m_stack.size(); i ++) {
                s.m_stack[i].print(os);
                if (i != s.m_stack.size() - 1)
                        os << ",";
        }
        // Goal for:
        os << ", goal_for:" << s.m_goal_for << std::endl;
        // Score:
        os << ", score: " << s.m_cur_score << "]";
}
