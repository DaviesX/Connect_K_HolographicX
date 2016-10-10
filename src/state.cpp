#include <ostream>
#include <iostream>
#include <log.h>
#include <iactioncost.h>
#include <iheuristic.h>
#include <state.h>


State::State(const unsigned num_cols, 
             const unsigned num_rows, 
             const bool gravity_on, 
             const Move& last_move,
             const unsigned k,
             const unsigned deadline,
             IActionCost* fcost,
             IHeuristic* heuristic):
        num_rows(num_rows), 
        num_cols(num_cols), 
        gravity_on(gravity_on),
        last_move(last_move),
        k(k),
        deadline(deadline),
        cost(0.0f),
        m_fcost(fcost),
        m_heuristic(heuristic)
{
        m_game_state = new int [num_cols*num_rows];
}


State::~State()
{
	delete [] m_game_state;
}

const int State::is(unsigned x, unsigned y) const
{
        return m_game_state[x + y*num_cols];
}

void State::is(unsigned x, unsigned y, int who)
{
        m_game_state[x + y*num_cols] = who;
}

float State::g(unsigned x, unsigned y) const
{
        return cost + m_fcost->evaluate(*this, x, y);
}

float State::f(unsigned x, unsigned y) const
{
        return m_heuristic->evaluate(*this, x, y);
}

bool State::is_goal() const
{
}

const std::vector<State::MiniNode>& State::path() const
{
        return m_stack;
}

void State::push_move(unsigned x, unsigned y, int who)
{
        if (x == -1 && y == -1)
                return;
        m_stack.push_back(State::MiniNode(x, y, cost));
        m_game_state[x + y*num_cols] = who;
        cost = g(x, y);
}

void State::pop_move()
{
        const State::MiniNode& node = m_stack.back();
        m_game_state[node.x + node.y*num_cols] = NO_PIECE;
        m_stack.pop_back();
}

void State::reset()
{
        while (!m_stack.empty()) {
                pop_move();
        }
}

void State::print_dbg_info()
{
        std::ostream& out = ::get_log_stream();
        out << "State = " << std::endl;
        if (m_fcost != nullptr)
                m_fcost->print_dbg_info();
        else
                out << "no cost function";
        out << std::endl;
        
        if (m_heuristic != nullptr) 
                m_heuristic->print_dbg_info();
        else
                out << "no heuristic function";
        out << std::endl;
        ::flush();
}
