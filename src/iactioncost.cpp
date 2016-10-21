#include <ostream>
#include <log.h>
#include <iactioncost.h>


IActionCost::IActionCost()
{
}

IActionCost::~IActionCost()
{
}

void IActionCost::print(std::ostream& os) const
{
        os << "IActionCost = none" << std::endl;
}

std::ostream& operator<<(std::ostream& os, const IActionCost& ac)
{
        ac.print(os);
        return os;
}
