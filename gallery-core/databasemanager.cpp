#include "databasemanager.h"
#include <QSqlDatabase>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager singleton;
    return singleton;
}

DatabaseManager::DatabaseManager(const QString& path):
    mDatabase(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"))),
    albumDao(*mDatabase), pictureDao(*mDatabase)
    // nvm it's not a pointer it's a dereference ^^
{
mDatabase->setDatabaseName(path);
mDatabase->open();
albumDao.init();
pictureDao.init();
}

DatabaseManager::~DatabaseManager()
{
    mDatabase->close();
    delete mDatabase;
}
