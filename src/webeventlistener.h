#ifndef WEBEVENTLISTENER_H
#define WEBEVENTLISTENER_H

#include <QObject>
#include <QMap>
#include <QFutureWatcher>
#include <memory>

class WebEventListener : public QObject
{
    Q_OBJECT
public:
    explicit WebEventListener(QObject *parent = nullptr);
    ///pass the list of files with the request id.
    ///The same id was passed to the map to query the coordinates
    /// the coordinates will be reported with the same request Id so we can use it to tag the scheduled files
    /// @see onLocationAcquired
    void scheduleFiles(uint requestId, const QStringList& files);
    bool isFinished() const;

public slots:
    ///call from the javascript to report the requested coordinates, and the request id
    /// that was passed during the java script function call
    void onLocationAcquired(double lat, double lng, uint requestId);
    void onMarkerClicked(const QString& fileName);

signals:
    ///emitted after the given file is tagged with the given coordinates
    void fileChanged(const QString& fileName, double lat, double lng);
    ///emitted if a geotag marker was clicked on the map. @param fileName is the name of the marker belongs to
    void markerClicked(const QString& fileName);

private:
    QMap<uint, QStringList> mScheduledFiles;
    ///the list of watchers for running tasks. If it is not empty then some jobs are not finished yet
    std::vector<std::shared_ptr<QFutureWatcher<void>>> mWatchers;
};

#endif // WEBEVENTLISTENER_H
