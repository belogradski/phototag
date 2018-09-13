#ifndef WEBEVENTLISTENER_H
#define WEBEVENTLISTENER_H

#include <QObject>

class WebEventListener : public QObject
{
    Q_OBJECT
public:
    explicit WebEventListener(QObject *parent = nullptr);

public slots:
    void onLocationAcquired(double lat, double lng, const QString& fileName);
    void onMarkerClicked(const QString& fileName);

signals:
    void fileChanged(const QString& fileName);
    void markerClicked(const QString& fileName);
};

#endif // WEBEVENTLISTENER_H
