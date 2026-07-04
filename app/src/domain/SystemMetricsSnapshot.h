// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

#ifndef SYSTEMMETRICSSNAPSHOT_H
#define SYSTEMMETRICSSNAPSHOT_H

#include <QString>
#include <QVariantList>

/**
 * @brief CPU metrics captured during one monitor refresh.
 *
 * String fields are already formatted for the current QML API. Missing runtime
 * sources use safe fallback values instead of failing the refresh.
 */
struct CpuMetricsSnapshot {
    int usage = 0;
    int temperature = 0;
    QString clock = "N/A";
    QVariantList coreUsages;
    QVariantList temperatureHistory;
    QString loadAverage = "N/A";
};

/**
 * @brief GPU metrics captured during one monitor refresh.
 *
 * `memoryUsage` is `-1` when GPU memory information is unavailable.
 */
struct GpuMetricsSnapshot {
    int usage = 0;
    int temperature = 0;
    int memoryUsage = -1;
};

/**
 * @brief Memory metrics captured during one monitor refresh.
 */
struct MemoryMetricsSnapshot {
    int usage = 0;
    QString used = "0M";
    QString free = "0M";
    QString cache = "0M";
    int total = 0;
};

/**
 * @brief Storage and swap metrics captured during one monitor refresh.
 */
struct StorageMetricsSnapshot {
    int usage = 0;
    int temperature = 0;
    QString total = "0GB";
    QString used = "0GB";
    QString free = "0GB";
    int swapUsage = 0;
    QString swapTotal = "0M";
    QString swapUsed = "0M";
    QString swapFree = "0M";
    QString ioRead = "0 MB/s";
    QString ioWrite = "0 MB/s";
    QVariantList ioHistory;
};

/**
 * @brief Network metrics captured during one monitor refresh.
 */
struct NetworkMetricsSnapshot {
    QString interfaceName = "N/A";
    QString ipAddress = "N/A";
    QString macAddress = "N/A";
    QString uploadSpeed = "0 B/s";
    QString downloadSpeed = "0 B/s";
    QString packetRate = "0 pps";
    int activeConnections = 0;
    QVariantList uploadHistory;
    QVariantList downloadHistory;
};

/**
 * @brief Full metric snapshot returned by the metric service.
 *
 * The snapshot is the service/facade boundary: monitor classes fill it, and
 * SystemController applies it to QML-facing properties.
 */
struct SystemMetricsSnapshot {
    CpuMetricsSnapshot cpu;
    GpuMetricsSnapshot gpu;
    MemoryMetricsSnapshot memory;
    StorageMetricsSnapshot storage;
    NetworkMetricsSnapshot network;
};

#endif // SYSTEMMETRICSSNAPSHOT_H
