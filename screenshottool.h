#ifndef SCREENSHOTTOOL_H
#define SCREENSHOTTOOL_H

#include "screenshotimageprovider.h"
#include <QObject>
#include <QPixmap>
#include <QWebEngineView>
#include <QQueue>
class ScreenshotTool : public QObject
{
    Q_OBJECT
    //Exposing the URLs as a model.
    Q_PROPERTY(QStringList urlsModel READ urlsModel WRITE setUrlsModel NOTIFY urlsModelChanged)
public:
    ScreenshotTool(ScreenshotImageProvider *provider);
    QStringList urlsModel() const;
    void setUrlsModel(QStringList model);
signals:
    void screenshotTaken(QString url, QPixmap screenshot);
    void screenshotCompleted(QString url);
    void urlsModelChanged();
    void progressChanged(qreal progress);
    void clear();
private slots:
    void loadFinished(bool ok);
    void screenshotNow(QWebEngineView *webview);
    bool isWhite(QPixmap pixmap);
    void renderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode);
private:
    void screenShotSingleTarget(QString url,QWebEngineView *webview);
    //The property for the URLs model.
    QStringList mSuccessUrls;
    ScreenshotImageProvider *mProvider;
    QStringList mFailedUrls;
    QQueue<QString> mLoadedUrls;
    quint32 mMaxWebViews;
    quint32 mUrlsCount;
    QMap<QString, quint32> mRetries;
    quint32 mMaxRetries;
    quint32 mDelay;
    bool mStop;
public slots:
    //Public slots are the methods available in QML.
    void screenShotSingleTarget(QString url);
    void screenShotFileList(QString path);
    void loadFromProject(QString path);
    void saveToProject(QString path);
    void clearAll();
    void stop();
};

#endif // SCREENSHOTTOOL_H
