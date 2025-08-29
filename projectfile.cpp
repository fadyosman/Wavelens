#include "projectfile.h"
#include <QtSql/QSqlQuery>
#include <QBuffer>
#include <QVariant>
#include <QDebug>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>

ProjectFile::ProjectFile(QString fileName)
{
    db = QSqlDatabase::addDatabase( "QSQLITE" );
    db.setDatabaseName(fileName);
    db.open();
    QSqlQuery query = QSqlQuery(db);
    query.exec( "CREATE TABLE IF NOT EXISTS screenshots ( url TEXT, screenshot BLOB )" );
}

void ProjectFile::saveScreenshot(QString url, QPixmap pixmap)
{
    QByteArray inByteArray;
    QBuffer inBuffer( &inByteArray );
    inBuffer.open( QIODevice::WriteOnly );
    pixmap.save( &inBuffer, "PNG" ); // write inPixmap into inByteArray in PNG format.
    QSqlQuery query = QSqlQuery( db );
    query.prepare( "INSERT INTO screenshots (url, screenshot) VALUES (:url, :imageData)" );
    query.bindValue(":url", url);
    query.bindValue(":imageData", inByteArray);
    if( !query.exec() )
            qDebug() << "Error inserting image into table:\n" << query.lastError();
}

QMap<QString, QPixmap> ProjectFile::loadScreenshots()
{
    QSqlQuery query = QSqlQuery( db );
    if( !query.exec( "SELECT url,screenshot from screenshots" ))
        qDebug() << "Error getting image from table:\n" << query.lastError();
    int urlFieldIndex = query.record().indexOf("url");
    int pixmapFieldIndex = query.record().indexOf("screenshot");

    QMap<QString, QPixmap> map;

    while (query.next()) {
        QByteArray outByteArray = query.value(pixmapFieldIndex).toByteArray();
        QPixmap outPixmap = QPixmap();
        outPixmap.loadFromData(outByteArray);
        QString url = query.value(urlFieldIndex).toString();
        map[url] = outPixmap;
        //qDebug()<<"[*] Loaded "<<url<<" from db.";
    }
    return map;
}

void ProjectFile::close()
{
    db.close();
}
