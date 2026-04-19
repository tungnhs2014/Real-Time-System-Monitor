#include "WarningThresholdPolicy.h"

#include <cassert>

int main()
{
    assert(WarningThresholdPolicy::cpuLevel(69, 70, 90) == WarningLevel::Normal);
    assert(WarningThresholdPolicy::cpuLevel(70, 70, 90) == WarningLevel::Warning);
    assert(WarningThresholdPolicy::cpuLevel(89, 70, 90) == WarningLevel::Warning);
    assert(WarningThresholdPolicy::cpuLevel(90, 70, 90) == WarningLevel::Critical);

    assert(WarningThresholdPolicy::ramLevel(74, 75) == WarningLevel::Normal);
    assert(WarningThresholdPolicy::ramLevel(75, 75) == WarningLevel::Warning);

    return 0;
}
