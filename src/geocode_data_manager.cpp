#include "geocode_data_manager.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

GeocodeDataManager::GeocodeDataManager(QObject *parent) :
    QObject(parent)
{
    m_pNetworkAccessManager = new QNetworkAccessManager(this);
    connect(m_pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

void GeocodeDataManager::getCoordinates(const QString& address)
{
    QString url = QString("https://maps.google.com/maps/api/geocode/json?address=%1&sensor=false&language=en&key=YOUR_API_KEY").arg(address);
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(url)));
}

void GeocodeDataManager::replyFinished(QNetworkReply* reply)
{
    QString json = reply->readAll();
    qDebug() << "Reply = " << json;
    qDebug() << "URL = " << reply->url();
    QString strUrl = reply->url().toString();

    QJsonParseError err;
    QJsonDocument result = QJsonDocument::fromJson(json.toUtf8(), &err);
    if(result.isNull() || !result.isObject())
    {
        emit errorOccured(QString("Cannot convert to QJson object: %1\n%2").arg(err.errorString()).arg(json));
        return;
    }

    QString code;
    QJsonObject object = result.object();
    if(object.contains("status"))
        code = object["status"].toString();

    qDebug() << "Code" << code;
    if(code != "OK")
    {
        QString error = code;
        if(object.contains("error_message"))
            error = object["error_message"].toString();
        emit errorOccured(QString("Request failed: %1").arg(error));
        return;
    }

    QJsonArray results = object["results"].toArray();
    if(results.count() == 0)
    {
        emit errorOccured(QString("Cannot find any locations"));
        return;
    }

    double east  = results[0].toObject()["geometry"].toObject()["location"].toObject()["lng"].toDouble();
    double north = results[0].toObject()["geometry"].toObject()["location"].toObject()["lat"].toDouble();

    emit coordinatesReady(north, east);

    QString address = results[0].toObject()["formatted_address"].toString();
    emit addressReady(address);
}
