#include <istrategy.h>


IStrategy::IStrategy()
{
}

IStrategy::~IStrategy()
{
}

IActionCost* IStrategy::get_gxy() const
{
        return nullptr;
}

IHeuristic* IStrategy::get_fxy() const
{
        return nullptr;
}
