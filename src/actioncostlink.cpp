#include <cmath>
#include <iostream>
#include <state.h>
#include <actioncostlink.h>


ActionCostLink::ActionCostLink()
{
}

ActionCostLink::~ActionCostLink()
{
        delete m_board;
}

// Helpers
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
        while (x >= 0 && x < m_w && y >= 0 && y < m_h &&
               is_markable(s, x, y, who)) {
                c ++;
                mark(x, y);
                x += dx;
                y += dy;
        }
        return c;
}

float ActionCostLink::score_links_for(const State& s, int who)
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

// Pulic APIs
void ActionCostLink::load_state(const State& s)
{
        // Evaluate the entire board for fresh start.
        m_board = new unsigned [s.num_cols*s.num_rows];
        for (unsigned i = 0; i < s.num_cols*s.num_rows; i ++)
                m_board[i] = 0;

        m_w = s.num_cols;
        m_h = s.num_rows;

        m_cur_score = score_links_for(s, State::HUMAN_PIECE)/(0.01 + score_links_for(s, State::AI_PIECE));
}

void ActionCostLink::accept(const State& s, float score, unsigned x, unsigned y)
{
        m_cur_score = score;
}

float ActionCostLink::evaluate(const State& k, unsigned x, unsigned y)
{
        return 0.0f;
}

void ActionCostLink::print(std::ostream& os) const
{
        os << "ActionCostLink = [marker map: " << std::endl;
        for (unsigned y = 0; y < m_h; y ++) {
                for (unsigned x = 0; x < m_w; x ++) {
                        os << m_board[x + y*m_w] << " ";
                }
                os << std::endl;
        }
        os << ", marker: " << m_marker << std::endl;
        os << ", score: " << m_cur_score << std::endl;
        os << "]";
}

std::ostream& operator<<(std::ostream& os, const ActionCostLink& ac)
{
        ac.print(os);
        return os;
}

