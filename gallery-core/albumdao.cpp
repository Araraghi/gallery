#include "albumdao.h"
#include "album.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>


AlbumDao::AlbumDao(QSqlDatabase& database):
    mDatabase(database)
{
}

void AlbumDao::init() const
{
// Code below may prove some diffuculties later
    if (!mDatabase.tables().contains("albums"))
    {
        QSqlQuery query(mDatabase);
        query.exec("CREATE TABLE albums(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT)");
    }
}

void AlbumDao::addAlbum(Album& album) const
{
    QSqlQuery query(mDatabase);
    query.prepare("INSERT INTO albums (name) VALUES (:name)");
    query.bindValue(":name", album.name());
    query.exec();
    album.setId(query.lastInsertId().toInt());
}

void AlbumDao::updateAlbum(const Album &album) const
{
    QSqlQuery query(mDatabase);
    query.prepare("UPDATE albums SET name = (:name) WHERE id = (:id)");
    query.bindValue(":name", album.name());
    query.bindValue(":id", album.id());
    query.exec();
}

void AlbumDao::removeAlbum(int id) const
{
    QSqlQuery query(mDatabase);
    query.prepare("DELETE FROM albums WHERE id = (:id)");
    query.bindValue(":id", id);
    query.exec();
}

std::unique_ptr<std::vector<std::unique_ptr<Album>>> AlbumDao::albums() const
{
    QSqlQuery query("SELECT * FROM albums",mDatabase);
    query.exec();
    auto list = std::make_unique<std::vector<std::unique_ptr<Album>>>();
    while(query.next())
    {
        auto album = std::make_unique<Album>();
        album->setId(query.value("id").toInt());
        album->setName(query.value("name").toString());
        list->push_back(std::move(album));
    }
    return list;
}

