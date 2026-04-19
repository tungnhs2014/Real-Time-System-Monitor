#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>
#include <QColor>

namespace App {

namespace Info {
    constexpr const char* NAME = "ILI9341 System Monitor";
    constexpr const char* VERSION = "4.0.0";
    constexpr const char* ORGANIZATION = "ILI9341";
    constexpr const char* DOMAIN = "ili9341.embedded";
}

namespace Interval {
    constexpr int FAST = 500;       // CPU, RAM real-time
    constexpr int NORMAL = 1000;    // Default update
    constexpr int SLOW = 2000;      // Network, Storage
    constexpr int VERY_SLOW = 5000; // System info
}

namespace Path {
    // CPU
    constexpr const char* PROC_STAT = "/proc/stat";
    constexpr const char* CPU_TEMP = "/sys/class/thermal/thermal_zone0/temp";
    constexpr const char* CPU_FREQ = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq";

    // Memory
    constexpr const char* PROC_MEMINFO = "/proc/meminfo";

    // Network
    constexpr const char* PROC_NET_DEV = "/proc/net/dev";
    constexpr const char* PROC_NET_TCP = "/proc/net/tcp";
    constexpr const char* PROC_NET_TCP6 = "/proc/net/tcp6";

    // System
    constexpr const char* PROC_UPTIME = "/proc/uptime";
    constexpr const char* PROC_VERSION = "/proc/version";
    constexpr const char* ETC_HOSTNAME = "/etc/hostname";
    constexpr const char* ETC_OS_RELEASE = "/etc/os-release";
}

namespace Threshold {
    constexpr int CPU_WARNING = 70;
    constexpr int CPU_CRITICAL = 90;

    constexpr int RAM_WARNING = 75;
    constexpr int RAM_CRITICAL = 90;

    constexpr int TEMP_WARNING = 60;
    constexpr int TEMP_CRITICAL = 80;

    constexpr int STORAGE_WARNING = 80;
    constexpr int STORAGE_CRITICAL = 95;
}

namespace NavIndex {
    constexpr int DASHBOARD = 0;
    constexpr int CPU = 1;
    constexpr int MEMORY = 2;
    constexpr int STORAGE = 3;
    constexpr int NETWORK = 4;
    constexpr int SETTINGS = 5;
}

} // namespace App

#endif // CONSTANTS_H
