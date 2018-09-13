#ifndef EXIFWRAPPER_H
#define EXIFWRAPPER_H
#include <QString>

namespace ExifWrapper
{
    bool isExifAvailable();
    bool hasGeoTags(const QString& fileName, double *lat, double* lng);
    int setGeoTags(double lat, double lng, const QString &fileName);
};

#endif // EXIFWRAPPER_H
