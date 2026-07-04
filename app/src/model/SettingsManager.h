// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QSettings>

/** Owns persisted user settings and platform-dependent system actions. */
class SettingsManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int updateInterval READ updateInterval WRITE setUpdateInterval NOTIFY settingsChanged)
    Q_PROPERTY(bool darkMode READ darkMode WRITE setDarkMode NOTIFY settingsChanged)
    Q_PROPERTY(bool soundAlert READ soundAlert WRITE setSoundAlert NOTIFY settingsChanged)

    Q_PROPERTY(int cpuWarnThreshold READ cpuWarnThreshold WRITE setCpuWarnThreshold NOTIFY settingsChanged)
    Q_PROPERTY(int cpuCritThreshold READ cpuCritThreshold WRITE setCpuCritThreshold NOTIFY settingsChanged)
    Q_PROPERTY(int ramWarnThreshold READ ramWarnThreshold WRITE setRamWarnThreshold NOTIFY settingsChanged)

    Q_PROPERTY(QString hostname READ hostname CONSTANT)
    Q_PROPERTY(QString osVersion READ osVersion CONSTANT)
    Q_PROPERTY(QString kernelVersion READ kernelVersion CONSTANT)
    Q_PROPERTY(QString uptime READ uptime NOTIFY uptimeChanged)
    Q_PROPERTY(QString systemTime READ systemTime NOTIFY systemTimeChanged)

    Q_PROPERTY(QVariantList systemLogs READ systemLogs NOTIFY logsChanged)

public:
    explicit SettingsManager(QObject* parent = nullptr);
    ~SettingsManager() override = default;

    int updateInterval() const { return m_updateInterval; }
    bool darkMode() const { return m_darkMode; }
    bool soundAlert() const { return m_soundAlert; }

    int cpuWarnThreshold() const { return m_cpuWarnThreshold; }
    int cpuCritThreshold() const { return m_cpuCritThreshold; }
    int ramWarnThreshold() const { return m_ramWarnThreshold; }

    QString hostname() const { return m_hostname; }
    QString osVersion() const { return m_osVersion; }
    QString kernelVersion() const { return m_kernelVersion; }
    QString uptime() const;
    QString systemTime() const;

    QVariantList systemLogs() const { return m_systemLogs; }

    void setUpdateInterval(int interval);
    void setDarkMode(bool enabled);
    void setSoundAlert(bool enabled);
    void setCpuWarnThreshold(int threshold);
    void setCpuCritThreshold(int threshold);
    void setRamWarnThreshold(int threshold);

    /** Persist the current settings through QSettings. */
    Q_INVOKABLE void save();
    /** Load settings using safe defaults when no persisted value exists. */
    Q_INVOKABLE void load();
    /** Request a system reboot; callers must obtain user confirmation first. */
    Q_INVOKABLE void reboot();
    /** Request a system shutdown; callers must obtain user confirmation first. */
    Q_INVOKABLE void shutdown();
    Q_INVOKABLE void addLog(const QString& level, const QString& message);
    Q_INVOKABLE void clearLogs();

signals:
    void settingsChanged();
    void logsChanged();
    void uptimeChanged();
    void systemTimeChanged();

private:
    QString parseHostname();
    QString parseOsVersion();
    QString parseKernelVersion();
    QString parseUptime() const;
    QString formatUptime(unsigned long long seconds) const;

private:
    int m_updateInterval;
    bool m_darkMode;
    bool m_soundAlert;

    QString m_hostname;
    QString m_osVersion;
    QString m_kernelVersion;

    int m_cpuWarnThreshold;
    int m_cpuCritThreshold;
    int m_ramWarnThreshold;

    QVariantList m_systemLogs;

    static constexpr int MAX_LOGS = 100;
};

#endif // SETTINGSMANAGER_H
