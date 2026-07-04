// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

#ifndef SYSTEMMETRICSSERVICE_H
#define SYSTEMMETRICSSERVICE_H

#include <QObject>

#include "SystemMetricsSnapshot.h"

class CpuMonitor;
class GpuMonitor;
class MemoryMonitor;
class StorageMonitor;
class NetworkMonitor;

/**
 * @class SystemMetricsService
 * @brief Owns monitor objects and returns typed snapshots for the facade.
 *
 * The service and its monitors are QObject children of this object and run on
 * the caller's thread. It preserves the existing polling behavior, including
 * monitor history updates and fallback values for missing `/proc`, `/sys`, or
 * command output.
 */
class SystemMetricsService : public QObject
{
    Q_OBJECT

public:
    explicit SystemMetricsService(QObject* parent = nullptr);
    ~SystemMetricsService() override = default;

    /**
     * @brief Collect a complete near-real-time snapshot from monitor sources.
     * @return Typed values ready for SystemController to expose to QML.
     */
    SystemMetricsSnapshot collectSnapshot();

private:
    CpuMetricsSnapshot collectCpuSnapshot();
    GpuMetricsSnapshot collectGpuSnapshot(int cpuUsage);
    MemoryMetricsSnapshot collectMemorySnapshot();
    StorageMetricsSnapshot collectStorageSnapshot();
    NetworkMetricsSnapshot collectNetworkSnapshot();

private:
    CpuMonitor* m_cpuMonitor;
    GpuMonitor* m_gpuMonitor;
    MemoryMonitor* m_memoryMonitor;
    StorageMonitor* m_storageMonitor;
    NetworkMonitor* m_networkMonitor;
};

#endif // SYSTEMMETRICSSERVICE_H
