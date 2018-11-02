#include "webeventlistener.h"
#include <QProcess>
#include <exifwrapper.h>
#include <QtConcurrent>

WebEventListener::WebEventListener(QObject *parent) : QObject(parent)
{

}

void WebEventListener::onLocationAcquired(double lat, double lng, uint requestId)
{
    for(const auto& file : mScheduledFiles[requestId])
    {
        auto watcher = std::make_shared<QFutureWatcher<void>>(new QFutureWatcher<void>);
        mWatchers.push_back(watcher);
        connect(watcher.get(), &QFutureWatcher<void>::finished, [&]() {
            //remove all finished watchers from the list
            mWatchers.erase(std::remove_if(mWatchers.begin(), mWatchers.end(), [](const std::shared_ptr<QFutureWatcher<void>>& f) { return f->isFinished();}), mWatchers.end());
        });

        //patch the file in a background thread
        QFuture<void> future = QtConcurrent::run([=](){
            ExifWrapper::setGeoTags(lat, lng, file);
            emit fileChanged(file, lat, lng);
        });

        watcher->setFuture(future);
    }
}

void WebEventListener::onMarkerClicked(const QString &fileName)
{
    emit markerClicked(fileName);
}

void WebEventListener::scheduleFiles(uint requestId, const QStringList &files)
{
    mScheduledFiles[requestId] = files;
}

bool WebEventListener::isFinished() const
{
    return mWatchers.empty();
}
