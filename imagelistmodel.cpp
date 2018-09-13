#include "imagelistmodel.h"
#include <QDir>
#include <QThread>
#include <QMimeData>
#include <QPainter>
#include <exifwrapper.h>
#include <QBuffer>

#define IMAGE_SIZE 128
#define TOOLTIP_SIZE 512


class WorkerThread : public QThread
{
public:
    WorkerThread(const WorkerThread* other)
        : mModel(other->mModel)
        , mCanceled(other->mCanceled)
    {
    }

    WorkerThread(ImageListModel* model)
        : mModel(model)
        , mCanceled(false)
    {
    }

    void addImages(const std::vector<QString>& files)
    {
        if (isRunning())
            cancel();
        wait();

        mCanceled = false;
        mFiles = files;
        start();
    }

protected:
    void run() override
    {
        auto it = mFiles.begin();
        while(!mCanceled && it != mFiles.end())
        {
            mModel->addImage(*it);
            it++;
        }
    }

private:
    ImageListModel* mModel;
    QString mPath;
    bool mCanceled;
    std::vector<QString> mFiles;

    void cancel()
    {
        mCanceled = true;
    }
};



ImageListModel::ImageListModel()
{
    for(int i=0; i<QThread::idealThreadCount(); i++)
        mWorkerThreads.push_back(std::make_unique<WorkerThread>(new WorkerThread(this)));
    mModelMutex = new QMutex(QMutex::Recursive);
}

void ImageListModel::resetModel()
{
    QMutexLocker ml(mModelMutex);
    beginResetModel();
    mImages.clear();
    endResetModel();
}

void ImageListModel::addImage(const QString& fileName)
{
   auto item = readFile(fileName);
   if(!item.image.isNull())
   {
       QMutexLocker ml(mModelMutex);
       auto start = mImages.size();
       beginInsertRows(QModelIndex(), start, start);
       mImages.push_back(item);
       endInsertRows();
   }
}


ImageListModel::ModelItem ImageListModel::readFile(const QString &filePath) const
{
    ModelItem item;
    item.filePath = filePath;

    QPixmap pm(filePath);
    if(!pm.isNull())
    {
        item.highresImage = pm.scaled(QSize(TOOLTIP_SIZE, TOOLTIP_SIZE), Qt::KeepAspectRatio);
        pm = pm.scaled(QSize(IMAGE_SIZE, IMAGE_SIZE), Qt::KeepAspectRatio);
        const static QPixmap tag = QPixmap(":/img/tag.png").scaled(32,32);
        item.hasGeoTag = ExifWrapper::hasGeoTags(filePath, &item.lat, &item.lng);
        if(item.hasGeoTag)
        {
            QPainter painter(&pm);
            painter.drawPixmap(tag.rect(), tag);
            item.hasGeoTag = true;
        }
        item.image = pm;
    }
    return item;
}

void ImageListModel::onFileChanged(const QString &filePath)
{
    for(int row = 0; row<rowCount(); row++)
    {
        auto fName = data(index(row, 0), Qt::DisplayRole).toString();
        if(filePath == fName)
        {
            auto item = readFile(filePath);
            if(!item.image.isNull())
                mImages[row] = item;

            emit dataChanged(index(row, 0),index(row, 0));
        }
    }
}

void ImageListModel::setDirectory(const QString &path)
{
    QDir dir(path);
    dir.setFilter(QDir::Files);
    QStringList files;
    for(const auto& fileInfo : dir.entryInfoList())
    {
        if(!fileInfo.suffix().endsWith("_original"))
            files.push_back(fileInfo.absoluteFilePath());
    }
    resetModel();
    int numOfThreads = mWorkerThreads.size();
    std::vector<std::vector<QString>> workload;
    for(int i=0; i<numOfThreads; i++)
    {
        workload.push_back({});
    }
    for(int i=0; i<files.size(); i++)
    {
        workload[i % numOfThreads].push_back(files[i]);
    }
    for(int i=0; i<numOfThreads; i++)
    {
        mWorkerThreads[i]->addImages(workload[i]);
    }
}

Qt::ItemFlags ImageListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | defaultFlags;
    else
        return Qt::ItemIsDropEnabled | defaultFlags;
}

int ImageListModel::rowCount(const QModelIndex &parent) const
{
    QMutexLocker ml(mModelMutex);
    return mImages.size();
}

QVariant ImageListModel::data(const QModelIndex &index, int role) const
{
    QVariant res;
    if(!index.isValid())
        return res;

    switch(role)
    {
    case Qt::DecorationRole:
        {
            QMutexLocker ml(mModelMutex);
            if(index.row()<mImages.size())
                res = mImages.at(index.row()).image;
        }
        break;
    case Qt::SizeHintRole:
        res = QSize(IMAGE_SIZE, IMAGE_SIZE);
        break;
    case Qt::DisplayRole:
        {
            QMutexLocker ml(mModelMutex);
            if(index.row()<mImages.size())
                res = mImages.at(index.row()).filePath;
        }
        break;
    case Qt::ToolTipRole:
        {
            QMutexLocker ml(mModelMutex);
            if(index.row()<mImages.size())
            {
                QPixmap pm = mImages.at(index.row()).highresImage;
//                QPixmap pm(mImages.at(index.row()).filePath);
  //              pm = pm.scaled(QSize(TOOLTIP_SIZE, TOOLTIP_SIZE), Qt::KeepAspectRatio);
                QByteArray data;
                QBuffer buffer(&data);
                pm.save(&buffer, "PNG", 100);
                res = QString("<img src='data:image/png;base64, %1'><br>%2").arg(QString(data.toBase64())).arg(mImages.at(index.row()).filePath);
            }
        }
        break;
    case ImageListModel::LongitudeRole:
        {
            QMutexLocker ml(mModelMutex);
            if(index.row()<mImages.size())
                res = mImages.at(index.row()).lng;
        }
        break;
    case ImageListModel::LatitudeRole:
        {
            QMutexLocker ml(mModelMutex);
            if(index.row()<mImages.size())
                res = mImages.at(index.row()).lat;
        }
        break;
    case ImageListModel::HasGeoTag:
        {
            QMutexLocker ml(mModelMutex);
            if(index.row()<mImages.size())
                res = mImages.at(index.row()).hasGeoTag;
        }
        break;

    }

    return res;
}

QStringList ImageListModel::mimeTypes() const
{
    QStringList types;
    types << "text/plain";
    return types;
}

QMimeData* ImageListModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    QStringList files;
    foreach (const QModelIndex &index, indexes) {
        if (index.isValid()) {
            files << data(index, Qt::DisplayRole).toString();
        }
    }

    if(!files.empty())
        mimeData->setText(files.join("|"));
 //   mimeData->setData("text/plain", encodedData);
    return mimeData;
}

