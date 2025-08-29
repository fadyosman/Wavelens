#include "screenshotwebenginepage.h"
#include <QWebEngineCertificateError>
#include <QTimer>
ScreenshotWebEnginePage::ScreenshotWebEnginePage(QObject *parent) : QWebEnginePage(parent)
{
    mErrorHappened = false;
}

ScreenshotWebEnginePage::ScreenshotWebEnginePage(QWebEngineProfile *profile) :QWebEnginePage(profile)
{
    mErrorHappened = false;
}

bool ScreenshotWebEnginePage::errorHappened()
{
    return mErrorHappened;
}

bool ScreenshotWebEnginePage::certificateError(const QWebEngineCertificateError &error)
{
    Q_UNUSED(error)
    mErrorHappened = true;
    return true;
}

void ScreenshotWebEnginePage::javaScriptConsoleMessage(QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID)
{
    Q_UNUSED(level)
    Q_UNUSED(message)
    Q_UNUSED(message);
    Q_UNUSED(lineNumber)
    Q_UNUSED(sourceID)
}
