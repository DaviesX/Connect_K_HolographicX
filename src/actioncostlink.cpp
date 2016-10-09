#include <iostream>
#include <state.h>
#include <actioncostlink.h>


ActionCostLink::ActionCostLink()
{
}

bool ActionCostLink::is_markable(const State& s, const unsigned x, const unsigned y, const int who) const
{
        return m_board[x + y*m_w] != m_marker && s.is(x, y) == who;
}

void ActionCostLink::mark(const unsigned x, const unsigned y)
{
        m_board[x + y*m_w] = m_marker;
}

void ActionCostLink::new_marker()
{
        m_marker ++;
}

unsigned ActionCostLink::trace(const State& s, const int who, int x, int y, const int dx, const int dy)
{
        unsigned c = 0;
        while (x >= 0 && x < m_w && y >= 0 && y < m_w &&
               is_markable(s, x, y, who)) {
                c ++;
                x += dx;
                y += dy;
        }
        return c;
}

float ActionCostLink::evaluate(const State& s, const int who)
{
        float score = 0;

        new_marker();
        for (unsigned y = 0; y < m_h; y ++) {
                for (unsigned x = 0; x < m_w; x ++) {
                        if (is_markable(s, x, y, who)) {
                                score += trace(s, who, x, y, 1, 0);
                        }
                }
        }
        new_marker();
        for (unsigned y = 0; y < m_h; y ++) {
                for (unsigned x = 0; x < m_w; x ++) {
                        if (is_markable(s, x, y, who)) {
                                score += trace(s, who, x, y, 0, 1);
                        }
                }
        }
        new_marker();
        for (unsigned y = 0; y < m_h; y ++) {
                for (unsigned x = 0; x < m_w; x ++) {
                        if (is_markable(s, x, y, who)) {
                                score += trace(s, who, x, y, 1, 1);
                        }
                }
        }
        new_marker();
        for (unsigned y = 0; y < m_h; y ++) {
                for (unsigned x = 0; x < m_w; x ++) {
                        if (is_markable(s, x, y, who)) {
                                score += trace(s, who, x, y, -1, 1);
                        }
                }
        }

        return score;
}

float ActionCostLink::evaluate(const State& k, unsigned x, unsigned y)
{
        State& s = (State&) k;
        s.push_move(x, y, State::AI_PIECE);
        if (m_board == nullptr) {
                m_board = new unsigned [s.num_cols*s.num_rows];
                for (unsigned i = 0; i < s.num_cols*s.num_rows; i ++)
                        m_board[i] = 0;
        } else if (m_w*m_h < s.num_cols*s.num_rows) {
                delete [] m_board;
                m_board = new unsigned [s.num_cols*s.num_rows];
                for (unsigned i = 0; i < s.num_cols*s.num_rows; i ++)
                        m_board[i] = 0;
        }
        m_w = s.num_cols;
        m_h = s.num_rows;
        s.pop_move();
        return evaluate(s, State::HUMAN_PIECE)/(0.00001 + evaluate(s, State::AI_PIECE));
}

void ActionCostLink::print_dbg_info() const
{
        std::cout << "ActionCostLink: marker map = " << std::endl;
        for (unsigned y = 0; y < m_h; y ++) {
                for (unsigned x = 0; x < m_w; x ++) {
                        std::cout << m_board[x + y*m_w] << " ";
                }
                std::cout << std::endl;
        }
        std::cout << "ActionCostLink: marker = " << m_marker << std::endl;
}

