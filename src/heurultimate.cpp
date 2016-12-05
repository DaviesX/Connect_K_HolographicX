#include "heurultimate.h"


HeuristicUltimate::HeuristicUltimate()
{
}

HeuristicUltimate::~HeuristicUltimate()
{
        clear_cnn();
        delete [] m_cnn_buf;
        delete [] m_cnn_map;
}

const HeuristicUltimate::ConnectedComponent& HeuristicUltimate::allocate_cnn(unsigned short& ref)
{
        ref = m_num_cnn;
        return m_cnn_buf[m_num_cnn ++];
}

const HeuristicUltimate::ConnectedComponent& HeuristicUltimate::obtain_cnn(unsigned short ref) const
{
        return m_cnn_buf[ref];
}

const HeuristicUltimate::ConnectedComponent& HeuristicUltimate::obtain_root_cnn(unsigned short ref) const
{
        while (obtain_cnn(ref).parent != ref)
                ref = obtain_cnn(ref).parent;
        return m_cnn_buf[ref];
}

void HeuristicUltimate::clear_cnn()
{
        m_num_cnn = 0;
}

const HeuristicUltimate::ComponentReference& HeuristicUltimate::cnn_map(unsigned char x, unsigned char y) const
{
        return m_cnn_map[x + y*m_cnn_w];
}

HeuristicUltimate::ComponentReference* HeuristicUltimate::cnn_map_m(unsigned char x, unsigned char y)
{
        return &m_cnn_map[x + y*m_cnn_w];
}

void HeuristicUltimate::mark_dead_moves(const State& st, int who)
{
        bool has_started = false;
        int opponent = opponent_of(who);

        // Scan d = 0.
        for (unsigned j = 0; j < st.num_rows; j ++) {
                int s = -1;
                for (unsigned i = 0; i <= st.num_cols; i ++) {
                        if (i == st.num_cols || st.is(i, j) == opponent) {
                                if (has_started && (int) i - s - 1 < (int) st.k) {
                                        // Dead.
                                        for (int k = i - 1; k > s; k --) {
                                                *cnn_map_m(k, j)->parent_m(who, 0) = REF_DEAD_MOVE;
                                        }
                                }
                                has_started = false;
                                s = i;
                        } else
                                has_started = true;
                }
        }

        // Scan d = 2.
        has_started = false;
        for (unsigned i = 0; i < st.num_cols; i ++) {
                int s = -1;
                for (unsigned j = 0; j <= st.num_rows; j ++) {
                        if (j == st.num_rows || st.is(i, j) == opponent) {
                                if (has_started && (int) j - s - 1 < (int) st.k) {
                                        // Dead.
                                        for (int k = j - 1; k > s; k --) {
                                                *cnn_map_m(i, k)->parent_m(who, 2) = REF_DEAD_MOVE;
                                        }
                                }
                                has_started = false;
                                s = j;
                        } else
                                has_started = true;
                }
        }

        // Scan d = 3
        has_started = false;
        for (unsigned j = 0; j < st.num_rows; j ++) {
                int s = -1;
                for (unsigned i = 0, l = j; i <= st.num_cols && l <= st.num_rows; l ++, i ++) {
                        if (i == st.num_cols || l == st.num_rows || st.is(i, l) == opponent) {
                                if (has_started && (int) i - s - 1 < (int) st.k) {
                                        // Dead.
                                        for (int k = i - 1, m = l - 1; k > s; k --, m --) {
                                                *cnn_map_m(k, m)->parent_m(who, 3) = REF_DEAD_MOVE;
                                        }
                                }
                                has_started = false;
                                s = i;
                        } else
                                has_started = true;
                }
        }
        has_started = false;
        for (unsigned i = 0; i < st.num_cols; i ++) {
                int s = -1;
                for (unsigned l = i, j = 0; l <= st.num_cols && j <= st.num_rows; l ++, j ++) {
                        if (l == st.num_cols || j == st.num_rows || st.is(l, j) == opponent) {
                                if (has_started && (int) j - s - 1 < (int) st.k) {
                                        // Dead.
                                        for (int k = l - 1, m = j - 1; m > s; k --, m --) {
                                                *cnn_map_m(k, m)->parent_m(who, 3) = REF_DEAD_MOVE;
                                        }
                                }
                                has_started = false;
                                s = j;
                        } else
                                has_started = true;
                }
        }

        // Scan d = 1
        has_started = false;
        for (int j = (int) st.num_rows - 1; j >= 0; j --) {
                int s = -1;
                for (int i = 0, l = j; i <= (int) st.num_cols && l >= -1; l --, i ++) {
                        if (i == (int) st.num_cols || l == -1 || st.is(i, l) == opponent) {
                                if (has_started && (int) i - s - 1 < (int) st.k) {
                                        // Dead.
                                        for (int k = i - 1, m = l + 1; k > s; k --, m ++) {
                                                *cnn_map_m(k, m)->parent_m(who, 1) = REF_DEAD_MOVE;
                                        }
                                }
                                has_started = false;
                                s = i;
                        } else
                                has_started = true;
                }
        }
        has_started = false;
        for (unsigned i = 0; i < st.num_cols; i ++) {
                int s = st.num_rows;
                for (int l = i, j = (int) st.num_rows - 1; l <= (int) st.num_cols && j >= -1; l ++, j --) {
                        if (l == (int) st.num_cols || j == -1 || st.is(l, j) == opponent) {
                                if (has_started && (int) j - s - 1 < (int) st.k) {
                                        // Dead.
                                        for (int k = l - 1, m = j + 1; m > s; k --, m ++) {
                                                *cnn_map_m(k, m)->parent_m(who, 1) = REF_DEAD_MOVE;
                                        }
                                }
                                has_started = false;
                                s = j;
                        } else
                                has_started = true;
                }
        }
}

void HeuristicUltimate::extract_connected_components(const State& s, int who)
{
}

void HeuristicUltimate::load_state(const State& s)
{
        clear_cnn();
        delete [] m_cnn_buf;
        delete [] m_cnn_map;

        m_cnn_buf = new ConnectedComponent[s.num_cols*s.num_rows];
        m_cnn_map = new ComponentReference[s.num_cols*s.num_rows];
        m_cnn_w = s.num_cols;
        m_cnn_h = s.num_rows;

        mark_dead_moves(s, State::AI_PIECE);
        mark_dead_moves(s, State::HUMAN_PIECE);
        extract_connected_components(s, State::AI_PIECE);
        extract_connected_components(s, State::HUMAN_PIECE);
}

static bool linkage(const char* val, int x, int y, unsigned dist, void* data)
{
        HeuristicUltimate::LinkStat* ls = static_cast<HeuristicUltimate::LinkStat*>(data);
        if (*val == ls->who) {
                ls->ins ++;
                ls->state = 1;
                return true;
        } else if (*val == State::NO_PIECE) {
                if (ls->state != 0) {
                        return false;
                } else {
                        ls->del ++;
                        return true;
                }
        } else
                return false;
}

float HeuristicUltimate::benefit(const State& s, const Move& next_move,
                                    int who, int extra_moves) const
{
        LinkStat ls(who), ls2(who);

        float score = 0.0f;
        for (unsigned d = 0; d < 4; d ++) {
                int x = next_move.x;
                int y = next_move.y;

                if (cnn_map(x, y).parent(who, d) == REF_DEAD_MOVE)
                        continue;

                unsigned dist = s.move_xy(x, y, d, &::linkage, &ls);
                if (ls.ins != 0 || dist == 1) {
                        // Where there is a match.
                        if ((s.collides_edges(x, y, d) ||
                             s.is(x, y) == opponent_of(who)))
                                // Where the link is blocked.
                                ls.ins --;
                } else {
                        ls.del = 0;
                }

                x = next_move.x;
                y = next_move.y;
                dist = s.move_xy(x, y, (d + 4)%8, &::linkage, &ls2);
                if (ls2.ins != 0 || dist == 1) {
                        // Where there is a match.
                        if ((s.collides_edges(x, y, (d + 4)%8) ||
                             s.is(x, y) == opponent_of(who)))
                                // Where the link is blocked.
                                ls2.ins --;
                } else {
                        ls2.del = 0;
                }

                int exp = extra_moves + (int) (ls.ins + ls2.ins) - /*3.0f/4.0f**/((float) (ls.del + ls2.del));
                if (exp >= 0)
                        score += exp*exp*(1 << exp);

                ls.reset();
                ls2.reset();
        }
        return score;
}

void HeuristicUltimate::try_move(const State& s, const Move& m)
{
        m_stack.push_back(m);
}

void HeuristicUltimate::untry_move()
{
        m_stack.pop_back();
}

float HeuristicUltimate::evaluate_move(const State& s, const Move& move, int who) const
{
        if (who == State::HUMAN_PIECE)
                return benefit(s, move, who, 3);
        else
                return benefit(s, move, who, 2);
}

float HeuristicUltimate::coarse_eval(const State& s, const Move& next_move) const
{
        return evaluate(s, next_move);
}

float HeuristicUltimate::evaluate(const State& s, const Move& next_move) const
{
        float s0 = 0;
        float s1 = 0;

        float n0 = 1;
        float n1 = 1;

        std::vector<Move>& sequence = const_cast<std::vector<Move>&>(m_stack);
        sequence.push_back(next_move);
        for (unsigned i = 0; i < sequence.size(); i ++) {
                const Move& m = sequence[i];
                if ((i & 1) == 0) {
                        s0 += evaluate_move(s, m, State::AI_PIECE);
                        n0 ++;
                } else {
                        s1 += evaluate_move(s, m, State::HUMAN_PIECE);
                        n1 ++;
                }
        }
        sequence.pop_back();
        return s0/n0 - s1/n1;
}

void HeuristicUltimate::print(std::ostream& os) const
{
        os << "ai" << std::endl;
        for (unsigned j = 0; j < m_cnn_h; j ++) {
                for (unsigned i = 0; i < m_cnn_w; i ++) {
                        for (unsigned d = 0; d < 4; d ++) {
                                os << cnn_map(i, j).parent(State::AI_PIECE, d);
                                os << "|";
                        }
                        os << "  ";
                }
                os << std::endl;
        }

        os << "human" << std::endl;
        for (unsigned j = 0; j < m_cnn_h; j ++) {
                for (unsigned i = 0; i < m_cnn_w; i ++) {
                        for (unsigned d = 0; d < 4; d ++) {
                                os << cnn_map(i, j).parent(State::HUMAN_PIECE, d);
                                os << "|";
                        }
                        os << "  ";
                }
                os << std::endl;
        }
}
