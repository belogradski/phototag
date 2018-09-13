#ifndef GEOCODE_DATA_MANAGER_H
#define GEOCODE_DATA_MANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

/**
 * The class can be used to querry Google Maps API for some adress.
 * The request can be sent calling getCoordinates() with some adress as a string.
 * After we get the reply from Google Maps the signals with the coordinates and the formatted adress are emitted
 */
class GeocodeDataManager : public QObject
{
    Q_OBJECT
public:
    explicit GeocodeDataManager(QObject *parent = 0);

    void getCoordinates(const QString& address);

signals:
    void errorOccured(const QString&);
    void coordinatesReady(double lat, double lng);
    void addressReady(const QString&);

private slots:
    void replyFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager* m_pNetworkAccessManager;
};

#endif // GEOCODE_DATA_MANAGER_H
