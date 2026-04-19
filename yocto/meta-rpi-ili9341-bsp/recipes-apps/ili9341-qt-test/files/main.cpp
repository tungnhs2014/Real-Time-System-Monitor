/*
 * Qt6 QML Application for ILI9341
 * 
 * Author: TungNHS
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    if (!qEnvironmentVariableIsSet("QT_QPA_PLATFORM"))
        qputenv("QT_QPA_PLATFORM", "linuxfb:fb=/dev/fb1");
    
    QGuiApplication app(argc, argv);
    
    app.setApplicationName("ILI9341 Qt Test");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("TungNHS");
    
    QQmlApplicationEngine engine;
    
    engine.rootContext()->setContextProperty("appVersion", "1.0");
    
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    if (engine.rootObjects().isEmpty())
        return -1;
    
    return app.exec();
}
