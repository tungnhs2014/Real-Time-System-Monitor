// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

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
    , m_lastCpuWarningLevel(0)
    , m_lastRamWarningLevel(0)
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

    const QString uptime = m_settingsManager->uptime();
    if (m_uptime != uptime) {
        m_uptime = uptime;
        emit uptimeChanged();
    }

    const QString systemTime = m_settingsManager->systemTime();
    if (m_systemTime != systemTime) {
        m_systemTime = systemTime;
        emit systemTimeChanged();
    }

    checkThresholds();
}

void SystemController::applySnapshot(const SystemMetricsSnapshot& snapshot)
{
    const auto updateValue = [this](auto& field, const auto& value, auto notify) {
        if (field != value) {
            field = value;
            (this->*notify)();
        }
    };

    updateValue(m_cpuUsage, snapshot.cpu.usage, &SystemController::cpuUsageChanged);
    updateValue(m_cpuTemp, snapshot.cpu.temperature, &SystemController::cpuTempChanged);
    updateValue(m_cpuClock, snapshot.cpu.clock, &SystemController::cpuClockChanged);
    updateValue(m_coreUsages, snapshot.cpu.coreUsages, &SystemController::coreUsagesChanged);
    updateValue(m_tempHistory, snapshot.cpu.temperatureHistory, &SystemController::tempHistoryChanged);
    updateValue(m_loadAverage, snapshot.cpu.loadAverage, &SystemController::loadAverageChanged);

    updateValue(m_gpuTemp, snapshot.gpu.temperature, &SystemController::gpuTempChanged);
    updateValue(m_gpuMemUsage, snapshot.gpu.memoryUsage, &SystemController::gpuMemUsageChanged);
    updateValue(m_gpuUsage, snapshot.gpu.usage, &SystemController::gpuUsageChanged);

    updateValue(m_ramUsage, snapshot.memory.usage, &SystemController::ramUsageChanged);
    updateValue(m_ramUsed, snapshot.memory.used, &SystemController::ramUsedChanged);
    updateValue(m_ramFree, snapshot.memory.free, &SystemController::ramFreeChanged);
    updateValue(m_ramCache, snapshot.memory.cache, &SystemController::ramCacheChanged);
    updateValue(m_ramTotal, snapshot.memory.total, &SystemController::ramTotalChanged);

    updateValue(m_hddUsage, snapshot.storage.usage, &SystemController::hddUsageChanged);
    updateValue(m_hddTemp, snapshot.storage.temperature, &SystemController::hddTempChanged);
    updateValue(m_hddTotal, snapshot.storage.total, &SystemController::hddTotalChanged);
    updateValue(m_hddUsed, snapshot.storage.used, &SystemController::hddUsedChanged);
    updateValue(m_hddFree, snapshot.storage.free, &SystemController::hddFreeChanged);
    updateValue(m_swapUsage, snapshot.storage.swapUsage, &SystemController::swapUsageChanged);
    updateValue(m_swapTotal, snapshot.storage.swapTotal, &SystemController::swapTotalChanged);
    updateValue(m_swapUsed, snapshot.storage.swapUsed, &SystemController::swapUsedChanged);
    updateValue(m_swapFree, snapshot.storage.swapFree, &SystemController::swapFreeChanged);
    updateValue(m_ioRead, snapshot.storage.ioRead, &SystemController::ioReadChanged);
    updateValue(m_ioWrite, snapshot.storage.ioWrite, &SystemController::ioWriteChanged);
    updateValue(m_ioHistory, snapshot.storage.ioHistory, &SystemController::ioHistoryChanged);

    updateValue(m_networkInterface, snapshot.network.interfaceName, &SystemController::networkInterfaceChanged);
    updateValue(m_ipAddress, snapshot.network.ipAddress, &SystemController::ipAddressChanged);
    updateValue(m_macAddress, snapshot.network.macAddress, &SystemController::macAddressChanged);
    updateValue(m_netUpSpeed, snapshot.network.uploadSpeed, &SystemController::netUpSpeedChanged);
    updateValue(m_netDownSpeed, snapshot.network.downloadSpeed, &SystemController::netDownSpeedChanged);
    updateValue(m_packetRate, snapshot.network.packetRate, &SystemController::packetRateChanged);
    updateValue(m_activeConnections, snapshot.network.activeConnections, &SystemController::activeConnectionsChanged);
    updateValue(m_netUpHistory, snapshot.network.uploadHistory, &SystemController::netUpHistoryChanged);
    updateValue(m_netDownHistory, snapshot.network.downloadHistory, &SystemController::netDownHistoryChanged);
}

void SystemController::checkThresholds()
{
    const int cpuWarningLevel = static_cast<int>(
        WarningThresholdPolicy::cpuLevel(m_cpuUsage, m_cpuWarnThreshold, m_cpuCritThreshold)
    );

    if (cpuWarningLevel != m_lastCpuWarningLevel) {
        if (cpuWarningLevel == 2) {
            m_settingsManager->addLog("CRIT", QString("CPU usage critical: %1%").arg(m_cpuUsage));
        }
        else if (cpuWarningLevel == 1) {
            m_settingsManager->addLog("WARN", QString("CPU usage high: %1%").arg(m_cpuUsage));
        }
        m_lastCpuWarningLevel = cpuWarningLevel;
    }

    const int ramWarningLevel = static_cast<int>(
        WarningThresholdPolicy::ramLevel(m_ramUsage, m_ramWarnThreshold)
    );

    if (ramWarningLevel != m_lastRamWarningLevel) {
        if (ramWarningLevel == 1) {
            m_settingsManager->addLog("WARN", QString("RAM usage high: %1%").arg(m_ramUsage));
        }
        m_lastRamWarningLevel = ramWarningLevel;
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
