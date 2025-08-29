#ifndef SCREENSHOTWEBENGINEPAGE_H
#define SCREENSHOTWEBENGINEPAGE_H

#include <QWebEnginePage>

class ScreenshotWebEnginePage : public QWebEnginePage
{
    Q_OBJECT
public:
    explicit ScreenshotWebEnginePage(QObject *parent = 0);
    ScreenshotWebEnginePage(QWebEngineProfile *profile);
    //Fix an issue where the screenshot is empty after the page load.
    bool errorHappened();
protected:
    bool certificateError(const QWebEngineCertificateError &error);
    void javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID) override;
private:
    bool mErrorHappened;
};

#endif // SCREENSHOTWEBENGINEPAGE_H
