#include "screenshottool.h"
#include <QFile>
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#include <QTimer>
#include <QCoreApplication>
#include <QRgb>
#include "screenshotimageprovider.h"
#include "screenshotwebenginepage.h"
#include "projectfile.h"

ScreenshotTool::ScreenshotTool(ScreenshotImageProvider *provider) : mProvider(provider)
{
    mMaxWebViews = 20;
    mUrlsCount = 0;
    mMaxRetries = 3;
    mDelay = 5000;
}

QStringList ScreenshotTool::urlsModel() const
{
    return mSuccessUrls;
}

void ScreenshotTool::setUrlsModel(QStringList model)
{
    if(mSuccessUrls != model) {
        mSuccessUrls = model;
        emit urlsModelChanged();
    }
}

void ScreenshotTool::loadFromProject(QString path)
{
    //Clear all data, and emit a signal so the qmlimageprovider will clear the data as well.
    clearAll();

    ProjectFile proj(QUrl(path).toLocalFile());
    QMap<QString, QPixmap> map = proj.loadScreenshots();
    QMapIterator<QString, QPixmap> i(map);
    mUrlsCount = map.size();
    while (i.hasNext()) {
        i.next();
        mSuccessUrls<<i.key();
        mProvider->screenshotTaken(i.key(), i.value());
        qreal progress = (qreal)(mSuccessUrls.count())/(qreal)mUrlsCount;
        emit progressChanged(progress);
        emit screenshotTaken(i.key(), i.value());
        emit screenshotCompleted(i.key());
        emit urlsModelChanged();
    }
    proj.close();
}

void ScreenshotTool::saveToProject(QString path)
{
    ProjectFile proj(QUrl(path).toLocalFile());
    auto pixmaps = mProvider->pixmaps();
    for (auto [key, value] : pixmaps.asKeyValueRange()) {
        proj.saveScreenshot(key, value);
    }
    proj.close();
}

void ScreenshotTool::clearAll()
{
    mSuccessUrls.clear();
    mFailedUrls.clear();
    mLoadedUrls.clear();
    mRetries.clear();
    mProvider->clear();
    mUrlsCount = 0;
    mStop = false;
    emit clear();
    emit progressChanged(0.0);
}

void ScreenshotTool::screenShotSingleTarget(QString url)
{
    QWebEngineView *v = new QWebEngineView();
    connect(v, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
    connect(v, SIGNAL(renderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus,int)), this, SLOT(renderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus,int)));
    QWebEngineProfile *profile = new QWebEngineProfile();
    ScreenshotWebEnginePage *page = new ScreenshotWebEnginePage(profile);
    page->settings()->setAttribute(QWebEngineSettings::ShowScrollBars,false);
    v->resize(1024,1024);
    v->setPage(page);
    v->setAttribute(Qt::WA_DontShowOnScreen, true);
    v->load(QUrl(url));
    v->show();
}

void ScreenshotTool::screenShotFileList(QString path)
{
    clearAll();

    QFile inputFile(QUrl(path).toLocalFile());
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        while (!in.atEnd() && !mStop)
        {
            QString url = in.readLine();
            //Take screenshot.
            if(!mLoadedUrls.contains(url)) {
                //screenShotSingleTarget(url);
                QCoreApplication::processEvents();
                mLoadedUrls<<url;
                mUrlsCount++;
            } else {
                //qDebug()<<"Duplicate URL: "<<url<<", clean your list :)";
            }
        }
        inputFile.close();
    } else {
        qDebug()<<"Failed to open"<<path;
    }

    quint32 i = 0;
    for (i=0; i<mMaxWebViews;i++) {
        if(mLoadedUrls.isEmpty()) {
            break;
        }
        screenShotSingleTarget(mLoadedUrls.takeFirst());
    }
}

void ScreenshotTool::loadFinished(bool ok)
{
    QWebEngineView *v = dynamic_cast<QWebEngineView *>(sender());
    if(ok) {
        QTimer::singleShot(300,this,[this,v] () {screenshotNow(v); });
        //screenshotNow(v);
    } else {
        mFailedUrls<<v->page()->requestedUrl().toString();
        qreal progress = (qreal)(mSuccessUrls.count()+mFailedUrls.count())/(qreal)mUrlsCount;
        emit progressChanged(progress);
        if(!mLoadedUrls.isEmpty()) {
            v->load(mLoadedUrls.takeFirst());
        } else {
            qDebug()<<"Deleting";
            disconnect(v, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
            v->deleteLater();
        }
    }
}

void ScreenshotTool::screenshotNow(QWebEngineView *v)
{
    ScreenshotWebEnginePage *page = dynamic_cast<ScreenshotWebEnginePage *>(v->page());
    QString url = page->requestedUrl().toString();
    QPixmap pixmap = v->grab();
    //A good fix to the delay needed for some sites to load.
    //TODO: implement a max number of retries per site QMap<QString, int>, the string is the URL and the int is the number of retries.
    if (isWhite(pixmap)) {
        if(mRetries.contains(url)) {
            if(mRetries[url] <= mMaxRetries) {
                mRetries[url] = mRetries[url]+1;
                QTimer::singleShot(mDelay,this,[this,v] () {screenshotNow(v); });
                return;
            }
        } else {
            mRetries[url] = 1;
            QTimer::singleShot(mDelay,this,[this,v] () {screenshotNow(v); });
            return;
        }
    }
    mSuccessUrls<<url;
    qreal progress = (qreal)(mSuccessUrls.count()+mFailedUrls.count())/(qreal)mUrlsCount;
    emit progressChanged(progress);
    emit screenshotTaken(url, pixmap);
    emit screenshotCompleted(url);
    emit urlsModelChanged();
    if(!mLoadedUrls.isEmpty()) {
        v->load(mLoadedUrls.takeFirst());
    } else {
        qDebug()<<"Deleting";
        disconnect(v, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
        v->deleteLater();
    }
}

bool ScreenshotTool::isWhite(QPixmap pixmap)
{
    QImage image = pixmap.toImage();
    qDebug()<<image.colorCount();
    if(image.allGray()) {
        for(int x=0; x<image.width(); x++)
        {
            for(int y=0; y<image.height(); y++)
            {
                if (image.pixel(x,y) != QColor("white").rgb()) {
                    return false;
                }
            }
        }
    } else {
        return false;
    }
    return true;
}

void ScreenshotTool::renderProcessTerminated(QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode)
{
    Q_UNUSED(terminationStatus)
    Q_UNUSED(exitCode)
    qDebug()<<"OOPS something bad happened.";
}

void ScreenshotTool::stop() {
    mStop = true;
    emit progressChanged(1.0);
}
