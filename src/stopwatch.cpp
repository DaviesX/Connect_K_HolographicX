#include <ctime>
#include <stopwatch.h>


StopWatch::StopWatch()
{
}

unsigned StopWatch::begin(unsigned total_amount)
{
        m_total = total_amount;
        m_begin = clock();
        m_last = m_begin;
}

float StopWatch::dt() const
{
        return m_dt;
}

float StopWatch::check_point()
{
        clock_t incum = clock();
        float elapsed = (incum - m_last)/(float) CLOCKS_PER_SEC*1000;
        m_dt = elapsed/m_total;
        m_last = incum;
        float used = (incum - m_begin)/(float) CLOCKS_PER_SEC*1000;
        return (m_total - used)/(float) m_total;
}
