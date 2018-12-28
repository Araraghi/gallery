#include "picturedao.h"
#include <QString>
#include <QSqlQuery>
#include <QVariant>
#include "picture.h"
PictureDao::PictureDao(QSqlDatabase& database):
    mDatabase(database)
{

}

void PictureDao::init() const
{
    if (!mDatabase.tables().contains("pictures"))
    {
        QSqlQuery query(mDatabase);
        query.exec(QString("CREATE TABLE pictures")
                + "(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                + "album_id INTEGER, url TEXT)");
    }
}

void PictureDao::addPictureInAlbum(int albumId, Picture &picture) const
{
    QSqlQuery query(mDatabase);
    query.prepare("INSERT INTO pictures(album_id, url) VALUES(:album_id, :url)");
    query.bindValue(":album_id", albumId);
    query.bindValue(":url", picture.fileUrl());
    query.exec();
    picture.setId(query.lastInsertId().toInt());
    picture.setAlbumId(albumId);
}

void PictureDao::removePicture(int id) const
{
     QSqlQuery query(mDatabase);
     query.prepare("DELETE FROM pictures WHERE id = (:id)");
     query.bindValue(":id", id);
     query.exec();
}

void PictureDao::removePicturesForAlbum(int albumId) const
{
     QSqlQuery query(mDatabase);
     query.prepare("DELETE FROM pictures WHERE album_id = (:album_id)");
     query.bindValue(":album_id", albumId);
     query.exec();
}

std::unique_ptr<std::vector<std::unique_ptr<Picture>>> PictureDao::picturesForAlbum(int albumId) const
{
    auto list{std::make_unique<std::vector<std::unique_ptr<Picture>>>()};
     QSqlQuery query(mDatabase);
     query.prepare("SELECT * FROM pictures WHERE album_id = (:album_id)");
     query.bindValue(":album_id", albumId);
     query.exec();
     while (query.next())
     {
         auto picture = std::make_unique<Picture>();
         picture->setFileUrl(query.value("url").toString());
         picture->setAlbumId(albumId);
         picture->setId(query.value("id").toInt());
         list->push_back(std::move(picture));
     }
     return list;
}
