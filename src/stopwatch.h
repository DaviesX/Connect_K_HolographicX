#ifndef STOPWATCH_H
#define STOPWATCH_H


#include <ctime>

/*
 * <StopWatch> So we know if we are running out of time.
 */
class StopWatch
{
public:
        StopWatch();
        unsigned        begin(unsigned total_amount);
        float           dt() const;
        float           check_point();
private:
        unsigned        m_total = 0;
        clock_t         m_begin = 0;
        clock_t         m_last = 0;
        float           m_dt = 0.0f;
};


#endif  // STOPWATCH_H
