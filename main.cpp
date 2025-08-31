#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include "screenshottool.h"
#include "screenshotimageprovider.h"

//Super straight forward, create interface and build the Tool as a plugin.
//https://blog.felgo.com/cross-platform-app-development/how-to-expose-a-qt-cpp-class-with-signals-and-slots-to-qml
//Using models is straight forward : https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quick/models/stringlistmodel/main.cpp?h=5.15
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS",QByteArray("--no-sandbox --enable-logging --log-level=3"));
    ScreenshotImageProvider *scImageProvider = new ScreenshotImageProvider();
    ScreenshotTool *scTool = new ScreenshotTool(scImageProvider);
    QObject::connect(scTool, SIGNAL(screenshotTaken(QString, QPixmap)), scImageProvider, SLOT(screenshotTaken(QString,QPixmap)));
    QQuickStyle::setStyle("Basic");
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.rootContext()->setContextProperty("screenshotTool", scTool);
    engine.addImageProvider(QLatin1String("screenshots"), scImageProvider);
    engine.load(url);
    return app.exec();
}
