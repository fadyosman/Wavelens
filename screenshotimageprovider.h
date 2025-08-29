#ifndef SCREENSHOTIMAGEPROVIDER_H
#define SCREENSHOTIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QMap>
#include <QPixmap>
#include <QQmlEngineExtensionPlugin>
class ScreenshotImageProvider : public QQuickImageProvider
{
    Q_OBJECT
public:
    ScreenshotImageProvider();
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;
    QMap<QString,QPixmap> pixmaps();
    QMap<QString,QPixmap> mPixmaps;
    void clear();
public slots:
    void screenshotTaken(QString id, QPixmap screenshot);
};

#endif // SCREENSHOTIMAGEPROVIDER_H
