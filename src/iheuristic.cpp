#include <log.h>
#include <iheuristic.h>


IHeuristic::IHeuristic()
{
}

void IHeuristic::print_dbg_info() const
{
        ::get_log_stream() << "IHeuristic = none" << std::endl;
}
