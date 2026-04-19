#include "SystemController.h"

#include "SettingsManager.h"
#include "SystemMetricsService.h"
#include "SystemMetricsSnapshot.h"
#include "WarningThresholdPolicy.h"

#include "Constants.h"
#include "Logger.h"
#include <QDebug>

SystemController::SystemController(QObject* parent)
    : QObject(parent)
    , m_metricsService(nullptr)
    , m_settingsManager(nullptr)
    , m_updateTimer(nullptr)
    , m_cpuUsage(0)
    , m_cpuTemp(0)
    , m_cpuClock("N/A")
    , m_loadAverage("N/A")
    , m_gpuUsage(0)
    , m_gpuTemp(0)
    , m_gpuMemUsage(-1)
    , m_ramUsage(0)
    , m_ramUsed("0M")
    , m_ramFree("0M")
    , m_ramCache("0M")
    , m_ramTotal(0)
    , m_hddUsage(0)
    , m_hddTemp(0)
    , m_hddTotal("0GB")
    , m_hddUsed("0GB")
    , m_hddFree("0GB")
    , m_swapUsage(0)
    , m_swapTotal("0M")
    , m_swapUsed("0M")
    , m_swapFree("0M")
    , m_ioRead("0 MB/s")
    , m_ioWrite("0 MB/s")
    , m_networkInterface("N/A")
    , m_ipAddress("N/A")
    , m_macAddress("N/A")
    , m_netUpSpeed("0 B/s")
    , m_netDownSpeed("0 B/s")
    , m_packetRate("0 pps")
    , m_activeConnections(0)
    , m_hostname("unknown")
    , m_osVersion("Unknown")
    , m_kernelVersion("Unknown")
    , m_uptime("0s")
    , m_systemTime("N/A")
    , m_updateInterval(2)
    , m_darkMode(true)
    , m_soundAlert(false)
    , m_cpuWarnThreshold(App::Threshold::CPU_WARNING)
    , m_cpuCritThreshold(App::Threshold::CPU_CRITICAL)
    , m_ramWarnThreshold(App::Threshold::RAM_WARNING)
{
    initializeServices();
    connectSignals();

    m_hostname = m_settingsManager->hostname();
    m_osVersion = m_settingsManager->osVersion();
    m_kernelVersion = m_settingsManager->kernelVersion();

    m_updateInterval = m_settingsManager->updateInterval();
    m_darkMode = m_settingsManager->darkMode();
    m_soundAlert = m_settingsManager->soundAlert();
    m_cpuWarnThreshold = m_settingsManager->cpuWarnThreshold();
    m_cpuCritThreshold = m_settingsManager->cpuCritThreshold();
    m_ramWarnThreshold = m_settingsManager->ramWarnThreshold();

    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(m_updateInterval * 1000);
    connect(m_updateTimer, &QTimer::timeout, this, &SystemController::updateAllMetrics);

    updateAllMetrics();

    m_updateTimer->start();

    LOG_INFO("SystemController initialized");
    LOG_INFO(QString("Update interval: %1 seconds").arg(m_updateInterval));
    LOG_INFO(QString("Hostname: %1").arg(m_hostname));
    LOG_INFO(QString("OS: %1").arg(m_osVersion));
    LOG_INFO(QString("Kernel: %1").arg(m_kernelVersion));
}

SystemController::~SystemController()
{
    if (m_updateTimer) {
        m_updateTimer->stop();
    }
}

void SystemController::initializeServices()
{
    m_metricsService = new SystemMetricsService(this);
    m_settingsManager = new SettingsManager(this);
}

void SystemController::connectSignals()
{
    connect(m_settingsManager, &SettingsManager::logsChanged, this, [this]() {
        m_systemLogs = m_settingsManager->systemLogs();
        emit systemLogsChanged();
    });

    m_systemLogs = m_settingsManager->systemLogs();
}

void SystemController::updateAllMetrics()
{
    const SystemMetricsSnapshot snapshot = m_metricsService->collectSnapshot();
    applySnapshot(snapshot);

    m_uptime = m_settingsManager->uptime();
    emit uptimeChanged();

    m_systemTime = m_settingsManager->systemTime();
    emit systemTimeChanged();

    checkThresholds();
}

void SystemController::applySnapshot(const SystemMetricsSnapshot& snapshot)
{
    if (m_cpuUsage != snapshot.cpu.usage) {
        m_cpuUsage = snapshot.cpu.usage;
        emit cpuUsageChanged();
    }

    if (m_cpuTemp != snapshot.cpu.temperature) {
        m_cpuTemp = snapshot.cpu.temperature;
        emit cpuTempChanged();
    }

    if (m_cpuClock != snapshot.cpu.clock) {
        m_cpuClock = snapshot.cpu.clock;
        emit cpuClockChanged();
    }

    m_coreUsages = snapshot.cpu.coreUsages;
    emit coreUsagesChanged();

    m_tempHistory = snapshot.cpu.temperatureHistory;
    emit tempHistoryChanged();

    if (m_loadAverage != snapshot.cpu.loadAverage) {
        m_loadAverage = snapshot.cpu.loadAverage;
        emit loadAverageChanged();
    }

    if (m_gpuTemp != snapshot.gpu.temperature) {
        m_gpuTemp = snapshot.gpu.temperature;
        emit gpuTempChanged();
    }

    if (m_gpuMemUsage != snapshot.gpu.memoryUsage) {
        m_gpuMemUsage = snapshot.gpu.memoryUsage;
        emit gpuMemUsageChanged();
    }

    if (m_gpuUsage != snapshot.gpu.usage) {
        m_gpuUsage = snapshot.gpu.usage;
        emit gpuUsageChanged();
    }

    if (m_ramUsage != snapshot.memory.usage) {
        m_ramUsage = snapshot.memory.usage;
        emit ramUsageChanged();
    }

    if (m_ramUsed != snapshot.memory.used) {
        m_ramUsed = snapshot.memory.used;
        emit ramUsedChanged();
    }

    if (m_ramFree != snapshot.memory.free) {
        m_ramFree = snapshot.memory.free;
        emit ramFreeChanged();
    }

    if (m_ramCache != snapshot.memory.cache) {
        m_ramCache = snapshot.memory.cache;
        emit ramCacheChanged();
    }

    if (m_ramTotal != snapshot.memory.total) {
        m_ramTotal = snapshot.memory.total;
        emit ramTotalChanged();
    }

    if (m_hddUsage != snapshot.storage.usage) {
        m_hddUsage = snapshot.storage.usage;
        emit hddUsageChanged();
    }

    if (m_hddTemp != snapshot.storage.temperature) {
        m_hddTemp = snapshot.storage.temperature;
        emit hddTempChanged();
    }

    if (m_hddTotal != snapshot.storage.total) {
        m_hddTotal = snapshot.storage.total;
        emit hddTotalChanged();
    }

    if (m_hddUsed != snapshot.storage.used) {
        m_hddUsed = snapshot.storage.used;
        emit hddUsedChanged();
    }

    if (m_hddFree != snapshot.storage.free) {
        m_hddFree = snapshot.storage.free;
        emit hddFreeChanged();
    }

    if (m_swapUsage != snapshot.storage.swapUsage) {
        m_swapUsage = snapshot.storage.swapUsage;
        emit swapUsageChanged();
    }

    m_swapTotal = snapshot.storage.swapTotal;
    emit swapTotalChanged();

    m_swapUsed = snapshot.storage.swapUsed;
    emit swapUsedChanged();

    m_swapFree = snapshot.storage.swapFree;
    emit swapFreeChanged();

    m_ioRead = snapshot.storage.ioRead;
    emit ioReadChanged();

    m_ioWrite = snapshot.storage.ioWrite;
    emit ioWriteChanged();

    m_ioHistory = snapshot.storage.ioHistory;
    emit ioHistoryChanged();

    m_networkInterface = snapshot.network.interfaceName;
    emit networkInterfaceChanged();

    m_ipAddress = snapshot.network.ipAddress;
    emit ipAddressChanged();

    m_macAddress = snapshot.network.macAddress;
    emit macAddressChanged();

    m_netUpSpeed = snapshot.network.uploadSpeed;
    emit netUpSpeedChanged();

    m_netDownSpeed = snapshot.network.downloadSpeed;
    emit netDownSpeedChanged();

    m_packetRate = snapshot.network.packetRate;
    emit packetRateChanged();

    m_activeConnections = snapshot.network.activeConnections;
    emit activeConnectionsChanged();

    m_netUpHistory = snapshot.network.uploadHistory;
    emit netUpHistoryChanged();

    m_netDownHistory = snapshot.network.downloadHistory;
    emit netDownHistoryChanged();
}

void SystemController::checkThresholds()
{
    static int lastCpuWarningLevel = 0;
    static int lastRamWarningLevel = 0;

    const int cpuWarningLevel = static_cast<int>(
        WarningThresholdPolicy::cpuLevel(m_cpuUsage, m_cpuWarnThreshold, m_cpuCritThreshold)
    );

    if (cpuWarningLevel != lastCpuWarningLevel) {
        if (cpuWarningLevel == 2) {
            m_settingsManager->addLog("CRIT", QString("CPU usage critical: %1%").arg(m_cpuUsage));
        }
        else if (cpuWarningLevel == 1) {
            m_settingsManager->addLog("WARN", QString("CPU usage high: %1%").arg(m_cpuUsage));
        }
        lastCpuWarningLevel = cpuWarningLevel;
    }

    const int ramWarningLevel = static_cast<int>(
        WarningThresholdPolicy::ramLevel(m_ramUsage, m_ramWarnThreshold)
    );

    if (ramWarningLevel != lastRamWarningLevel) {
        if (ramWarningLevel == 1) {
            m_settingsManager->addLog("WARN", QString("RAM usage high: %1%").arg(m_ramUsage));
        }
        lastRamWarningLevel = ramWarningLevel;
    }
}

void SystemController::setUpdateInterval(int interval)
{
    if (m_updateInterval != interval && interval >= 1 && interval <= 10) {
        m_updateInterval = interval;
        m_settingsManager->setUpdateInterval(interval);
        m_updateTimer->setInterval(interval * 1000);
        emit updateIntervalChanged();
    }
}

void SystemController::setDarkMode(bool enabled)
{
    if (m_darkMode != enabled) {
        m_darkMode = enabled;
        m_settingsManager->setDarkMode(enabled);
        emit darkModeChanged();
    }
}

void SystemController::setSoundAlert(bool enabled)
{
    if (m_soundAlert != enabled) {
        m_soundAlert = enabled;
        m_settingsManager->setSoundAlert(enabled);
        emit soundAlertChanged();
    }
}

void SystemController::setCpuWarnThreshold(int threshold)
{
    if (m_cpuWarnThreshold != threshold) {
        m_cpuWarnThreshold = threshold;
        m_settingsManager->setCpuWarnThreshold(threshold);
        emit cpuWarnThresholdChanged();
    }
}

void SystemController::setCpuCritThreshold(int threshold)
{
    if (m_cpuCritThreshold != threshold) {
        m_cpuCritThreshold = threshold;
        m_settingsManager->setCpuCritThreshold(threshold);
        emit cpuCritThresholdChanged();
    }
}

void SystemController::setRamWarnThreshold(int threshold)
{
    if (m_ramWarnThreshold != threshold) {
        m_ramWarnThreshold = threshold;
        m_settingsManager->setRamWarnThreshold(threshold);
        emit ramWarnThresholdChanged();
    }
}

void SystemController::saveSettings()
{
    m_settingsManager->save();
}

void SystemController::reboot()
{
    m_settingsManager->reboot();
}

void SystemController::shutdown()
{
    m_settingsManager->shutdown();
}

void SystemController::addLog(const QString& level, const QString& message)
{
    m_settingsManager->addLog(level, message);
}
