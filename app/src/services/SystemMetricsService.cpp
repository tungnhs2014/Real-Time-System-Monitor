#include "SystemMetricsService.h"

#include "CpuMonitor.h"
#include "GpuMonitor.h"
#include "MemoryMonitor.h"
#include "NetworkMonitor.h"
#include "StorageMonitor.h"

SystemMetricsService::SystemMetricsService(QObject* parent)
    : QObject(parent)
    , m_cpuMonitor(new CpuMonitor(this))
    , m_gpuMonitor(new GpuMonitor(this))
    , m_memoryMonitor(new MemoryMonitor(this))
    , m_storageMonitor(new StorageMonitor(this))
    , m_networkMonitor(new NetworkMonitor(this))
{
}

SystemMetricsSnapshot SystemMetricsService::collectSnapshot()
{
    SystemMetricsSnapshot snapshot;

    snapshot.cpu = collectCpuSnapshot();
    snapshot.gpu = collectGpuSnapshot(snapshot.cpu.usage);
    snapshot.memory = collectMemorySnapshot();
    snapshot.storage = collectStorageSnapshot();
    snapshot.network = collectNetworkSnapshot();

    return snapshot;
}

CpuMetricsSnapshot SystemMetricsService::collectCpuSnapshot()
{
    CpuMetricsSnapshot snapshot;

    snapshot.usage = m_cpuMonitor->parseUsage();
    snapshot.temperature = m_cpuMonitor->parseTemp();
    snapshot.clock = m_cpuMonitor->parseClock();
    snapshot.coreUsages = m_cpuMonitor->parsePerCoreUsage();
    m_cpuMonitor->updateTempHistory(snapshot.temperature);
    snapshot.temperatureHistory = m_cpuMonitor->getTempHistory();
    snapshot.loadAverage = m_cpuMonitor->parseLoadAverage();

    return snapshot;
}

GpuMetricsSnapshot SystemMetricsService::collectGpuSnapshot(int cpuUsage)
{
    GpuMetricsSnapshot snapshot;

    snapshot.temperature = m_gpuMonitor->parseTemp();
    snapshot.memoryUsage = m_gpuMonitor->parseMemUsage();
    snapshot.usage = cpuUsage;

    return snapshot;
}

MemoryMetricsSnapshot SystemMetricsService::collectMemorySnapshot()
{
    MemoryMetricsSnapshot snapshot;

    snapshot.usage = m_memoryMonitor->parseUsage();
    snapshot.used = m_memoryMonitor->parseUsed();
    snapshot.free = m_memoryMonitor->parseFree();
    snapshot.cache = m_memoryMonitor->parseCache();
    snapshot.total = m_memoryMonitor->parseTotal();

    return snapshot;
}

StorageMetricsSnapshot SystemMetricsService::collectStorageSnapshot()
{
    StorageMetricsSnapshot snapshot;

    snapshot.usage = m_storageMonitor->parseUsage();
    snapshot.temperature = m_storageMonitor->parseTemp();

    m_storageMonitor->updateRootPartition();
    snapshot.total = m_storageMonitor->getRootTotal();
    snapshot.used = m_storageMonitor->getRootUsed();
    snapshot.free = m_storageMonitor->getRootFree();

    m_storageMonitor->updateSwap();
    snapshot.swapUsage = m_storageMonitor->getSwapUsage();
    snapshot.swapTotal = m_storageMonitor->getSwapTotal();
    snapshot.swapUsed = m_storageMonitor->getSwapUsed();
    snapshot.swapFree = m_storageMonitor->getSwapFree();

    m_storageMonitor->updateIoStats();
    snapshot.ioRead = m_storageMonitor->getIoRead();
    snapshot.ioWrite = m_storageMonitor->getIoWrite();
    snapshot.ioHistory = m_storageMonitor->getIoHistory();

    return snapshot;
}

NetworkMetricsSnapshot SystemMetricsService::collectNetworkSnapshot()
{
    NetworkMetricsSnapshot snapshot;

    m_networkMonitor->update();

    snapshot.interfaceName = m_networkMonitor->getInterface();
    snapshot.ipAddress = m_networkMonitor->getIpAddress();
    snapshot.macAddress = m_networkMonitor->getMacAddress();
    snapshot.uploadSpeed = m_networkMonitor->getUpSpeed();
    snapshot.downloadSpeed = m_networkMonitor->getDownSpeed();
    snapshot.packetRate = m_networkMonitor->getPacketRate();
    snapshot.activeConnections = m_networkMonitor->getActiveConnections();
    snapshot.uploadHistory = m_networkMonitor->getUpHistory();
    snapshot.downloadHistory = m_networkMonitor->getDownHistory();

    return snapshot;
}
