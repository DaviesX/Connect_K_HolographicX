#include <iostream>
#include <log.h>
#include <iactioncost.h>


IActionCost::IActionCost()
{
}

void IActionCost::print_dbg_info() const
{
        ::get_log_stream() << "IActionCost = none" << std::endl;
}
