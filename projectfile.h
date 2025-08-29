#ifndef PROJECTFILE_H
#define PROJECTFILE_H
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QPixmap>
#include <QMap>

class ProjectFile
{
public:
    ProjectFile(QString fileName);
    void saveScreenshot(QString url, QPixmap pixmap);
    QMap<QString, QPixmap> loadScreenshots();
    void close();
private:
    QSqlDatabase db;
};

#endif // PROJECTFILE_H
