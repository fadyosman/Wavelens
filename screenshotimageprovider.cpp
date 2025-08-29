#include "screenshotimageprovider.h"
#include <QPainter>
ScreenshotImageProvider::ScreenshotImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap)
{

}

QPixmap ScreenshotImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize)
    if (size)
        *size = mPixmaps[id].size();
    return mPixmaps[id];
}

QMap<QString, QPixmap> ScreenshotImageProvider::pixmaps()
{
    return mPixmaps;
}

void ScreenshotImageProvider::clear()
{
    mPixmaps.clear();
}

void ScreenshotImageProvider::screenshotTaken(QString id, QPixmap screenshot)
{
    mPixmaps.insert(id,screenshot);
}
