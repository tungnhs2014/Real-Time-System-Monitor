// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

#ifndef WARNINGTHRESHOLDPOLICY_H
#define WARNINGTHRESHOLDPOLICY_H

/**
 * @brief Warning severity used by threshold checks.
 */
enum class WarningLevel {
    Normal = 0,
    Warning = 1,
    Critical = 2
};

/**
 * @brief Pure threshold policy used by the QML-facing facade.
 *
 * This class has no Qt or platform dependency so it can be tested without the
 * app runtime.
 */
class WarningThresholdPolicy
{
public:
    /**
     * @brief Classify CPU usage against warning and critical thresholds.
     */
    static WarningLevel cpuLevel(int usage, int warningThreshold, int criticalThreshold)
    {
        if (usage >= criticalThreshold) {
            return WarningLevel::Critical;
        }

        if (usage >= warningThreshold) {
            return WarningLevel::Warning;
        }

        return WarningLevel::Normal;
    }

    /**
     * @brief Classify RAM usage against the warning threshold.
     */
    static WarningLevel ramLevel(int usage, int warningThreshold)
    {
        return usage >= warningThreshold ? WarningLevel::Warning : WarningLevel::Normal;
    }

private:
    WarningThresholdPolicy() = default;
};

#endif // WARNINGTHRESHOLDPOLICY_H
