#include "exifwrapper.h"
#include <QProcess>
#include <QDebug>
#include <QFileInfo>

#define EXIF_APP "exiftool.exe"

namespace
{
    double stringCoordinatesToDecimal(const QString& coord)
    {
        auto items = coord.split(" ", QString::SkipEmptyParts);
        int deg = items.at(0).toInt();
        double min = items.at(2).left(items.at(2).size()-1).toDouble();
        double sec = items.at(3).left(items.at(3).size()-1).toDouble();
        int sign = (items.at(4) == "N" || items.at(4) == "E") ? 1 : -1;

        return sign*(deg + min/60 + sec/3600);
    }
}

namespace ExifWrapper
{
bool isExifAvailable()
{
    QFileInfo fi(EXIF_APP);
    return fi.isReadable();
}

bool hasGeoTags(const QString& fileName, double* lat, double* lng)
{
    QStringList args = { QString("-GPSLatitude"), QString("-GPSLongitude"), fileName};
    QProcess proc;
    proc.start(EXIF_APP, args);
    proc.waitForReadyRead();

    auto output = QString(proc.readAll()).split("\r\n", QString::SkipEmptyParts);
    if(output.size()!= 2)
        return false;

    if(lat)
        *lat = stringCoordinatesToDecimal(output.at(0).split(":").at(1));
    if(lng)
        *lng = stringCoordinatesToDecimal(output.at(1).split(":").at(1));

    return true;
}

int setGeoTags(double lat, double lng, const QString &fileName)
{
    QStringList args = { QString("-GPSLatitude=%1").arg(lat)
                         ,QString("-GPSLatitudeRef=%1").arg(lat >= 0 ? "N" : "S")
                         ,QString("-GPSLongitude=%1").arg(lng)
                         ,QString("-GPSLongitudeRef=%1").arg(lng >= 0 ?"E":"W")
                       ,fileName};

    int res = QProcess::execute(EXIF_APP, args);
    if(res == 0)
    {
        QString backup = fileName+"_original";
        QFile(backup).remove();
    }
    return res;
}
}
