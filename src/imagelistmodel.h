#ifndef IMAGELISTMODEL_H
#define IMAGELISTMODEL_H

#include <QAbstractListModel>
#include <QPixmap>
#include <QThread>
#include <QMutex>
#include <memory>

class ImageListModel : public QAbstractListModel
{
    friend class WorkerThread;
public:
    ImageListModel();
    ~ImageListModel();

    enum CustomRoles
    {
        LatitudeRole  = Qt::UserRole + 1,
        LongitudeRole = Qt::UserRole + 2,
        HasGeoTag     = Qt::UserRole + 3
    };
    void setDirectory(const QString& path);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QStringList mimeTypes() const override;
    QMimeData* mimeData(const QModelIndexList & indexes) const override;

    void onFileChanged(const QString &filePath, double lat, double lng);

private:
    struct ModelItem
    {
        QPixmap image; //the lowres image shown in the view table
        QPixmap highresImage; //is used for high res tooltip preview
        QString filePath;
        bool    hasGeoTag = false;
        double  lat = 0.;
        double  lng = 0.;
    };
    void resetModel();
    void addImage(const QString& fileName);
    ModelItem readFile(const QString& filePath) const;

    QList<ModelItem> mImages;
    std::vector<std::unique_ptr<WorkerThread>>  mWorkerThreads;
    mutable QMutex* mModelMutex;
};

#endif // IMAGELISTMODEL_H
