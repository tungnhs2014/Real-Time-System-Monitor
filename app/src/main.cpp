// SPDX-License-Identifier: GPL-2.0-only
// Copyright (c) 2025-2026 TungNHS

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QFont>
#include <QFile>
#include <QDir>

#include "controller/SystemController.h"

#include "common/Constants.h"
#include "common/Logger.h"

#ifdef PLATFORM_RASPBERRY_PI
// Prefer the XPT2046 input node, with event0 as a bring-up fallback.
QString findTouchDevice()
{
    for (int i = 0; i < 10; i++) {
        QString sysPath = QString("/sys/class/input/event%1/device/name").arg(i);

        QFile nameFile(sysPath);
        if (nameFile.open(QIODevice::ReadOnly | QIODevice:: Text)) {
            QString deviceName = QString::fromUtf8(nameFile.readAll()).trimmed();
            nameFile.close();

            qDebug() << "event" << i << ":" << deviceName;

            if (deviceName.contains("XPT2046", Qt::CaseInsensitive) ||
                deviceName.contains("ADS7846", Qt::CaseInsensitive) ||
                deviceName.contains("Touchscreen", Qt::CaseInsensitive)) {

                QString devicePath = QString("/dev/input/event%1").arg(i);
                qInfo() << "Touch device found:" << devicePath << "-" << deviceName;
                return devicePath;
            }
        }
    }
    qWarning() << "Touch device not found, using fallback: /dev/input/event0";
    return "/dev/input/event0";
}

#endif

int main(int argc, char *argv[])
{
#ifdef PLATFORM_RASPBERRY_PI
    qputenv("QT_QPA_PLATFORM", "linuxfb:fb=/dev/fb1:size=320x240:mmSize=60x45:offset=0x0");
    constexpr bool embeddedTarget = true;
    constexpr int uiAnimationDurationMs = 0;

    QString touchDevice = findTouchDevice();
    QByteArray touchParams = QString("%1:rotate=0:invertx=0:inverty=0")
                            .arg(touchDevice).toUtf8();
    qputenv("QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS", touchParams);
    qputenv("QT_QPA_GENERIC_PLUGINS", QString("evdevtouch:%1").arg(touchDevice).toUtf8());
    // Keep linuxfb from registering a second backend for the same XPT2046 node.
    qputenv("QT_QPA_FB_DISABLE_INPUT", "1");

    qDebug() << "Platform: Raspberry Pi - RGB565 mode";
    qDebug() << "Touch device:" << touchDevice;
#else
    constexpr bool embeddedTarget = false;
    constexpr int uiAnimationDurationMs = 180;
    qDebug() << "Platform: Desktop";
#endif

    QGuiApplication app(argc, argv);

    app.setOrganizationName(App::Info::ORGANIZATION);
    app.setOrganizationDomain(App::Info::DOMAIN);
    app.setApplicationName(App::Info::NAME);
    app.setApplicationVersion(App::Info::VERSION);

    QFont appFont;
    appFont.setFamily("DejaVu Sans");
    appFont.setHintingPreference(QFont::PreferFullHinting);
    app.setFont(appFont);

    Logger::instance().setMaxLogEntries(100);
    LOG_INFO("Application starting...");

    QQmlApplicationEngine engine;

    SystemController* systemController = new SystemController(&app);

    engine.rootContext()->setContextProperty("systemInfo", systemController);
    engine.rootContext()->setContextProperty("embeddedTarget", embeddedTarget);
    engine.rootContext()->setContextProperty("uiAnimationDuration", uiAnimationDurationMs);

#if LOG_LEVEL > LOG_LEVEL_OFF
    engine.rootContext()->setContextProperty("logger", &Logger::instance());
#endif

#ifdef QT_NO_DEBUG
    engine.rootContext()->setContextProperty("QML_DEBUG_ENABLED", false);
#else
    engine.rootContext()->setContextProperty("QML_DEBUG_ENABLED", true);
#endif

    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject* obj, const QUrl& objUrl) {
        if (!obj && url == objUrl) {
            qCritical() << "Failed to load QML";
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "No root objects loaded";
        return -1;
    }

    LOG_INFO("Application started successfully");
    qDebug() << App::Info::NAME << "v" << App::Info::VERSION;

    return app.exec();
}
