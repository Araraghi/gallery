#include "picturemodel.h"
#include "databasemanager.h"
#include "albummodel.h"
#include "picture.h"





PictureModel::PictureModel(const AlbumModel& albumModel, QObject* parent):
    QAbstractListModel(parent),
    mDb{DatabaseManager::instance()},
    mAlbumId(-1),
    mPictures(std::make_unique<std::vector<std::unique_ptr<Picture>>>())

{
    connect(&albumModel, &AlbumModel::rowsRemoved, this, &PictureModel::deletePicturesForAlbum);
}
void PictureModel::setAlbumId(int albumId)
{
    beginResetModel();
    mAlbumId = albumId;
    loadPictures(mAlbumId);
    endResetModel();
}

void PictureModel::loadPictures(int albumId)
{
    if (albumId <= 0)
    {
        mPictures.reset(new std::vector<std::unique_ptr<Picture>>());
    }
    mPictures = mDb.pictureDao.picturesForAlbum(albumId);
}

int PictureModel::rowCount(const QModelIndex &parent) const
{
    return mPictures->size();
}

QModelIndex PictureModel::addPicture(const Picture &picture)
{
    int rowIndex{rowCount()};
    beginInsertRows(QModelIndex(), rowIndex, rowIndex);
    std::unique_ptr<Picture> newPicture{std::make_unique<Picture>(picture)};
    mDb.pictureDao.addPictureInAlbum(mAlbumId, *newPicture);
    mPictures->push_back(std::move(newPicture));
    endInsertRows();
    return index(rowIndex, 0);
}

QVariant PictureModel::data(const QModelIndex &index, int role) const
{
    if (!isIndexValid(index)) return QVariant();
    const Picture& picture = *mPictures->at(index.row());
    switch (role)
    {
    case Qt::DisplayRole:
               return picture.fileUrl().fileName();
    case PictureRole::UrlRole:
               return picture.fileUrl();
    case PictureRole::FilePathRole:
               return picture.fileUrl().toLocalFile();

    default:
        return QVariant();
    }
}

bool PictureModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row > rowCount() || count < 0 || (row + count) > rowCount()) return false;

    beginRemoveRows(parent, row, row + count -1);
    int counter{count};
    while (counter--)
    {
        const Picture& picture = *mPictures->at(row + counter);
        mDb.pictureDao.removePicture(picture.id());
    }
    mPictures->erase(mPictures->begin() + row, mPictures->begin() + row + count);
    endRemoveRows();
    return true;
}

void PictureModel::clearAlbum()
{
    setAlbumId(-1);
}

void PictureModel::deletePicturesForAlbum()
{
    mDb.pictureDao.removePicturesForAlbum(mAlbumId);
    clearAlbum();
}

bool PictureModel::isIndexValid(const QModelIndex &index) const
{
    return !(index.row() < 0 || index.row() > rowCount() || !index.isValid());
    // slightly modified
}

