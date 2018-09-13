#include "webeventlistener.h"
#include <QProcess>
#include <exifwrapper.h>

WebEventListener::WebEventListener(QObject *parent) : QObject(parent)
{

}

void WebEventListener::onLocationAcquired(double lat, double lng, const QString &fileName)
{
    ExifWrapper::setGeoTags(lat, lng, fileName);

    emit fileChanged(fileName);
}

void WebEventListener::onMarkerClicked(const QString &fileName)
{
    emit markerClicked(fileName);
}
