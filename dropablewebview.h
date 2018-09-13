#ifndef QDROPABLEWEBVIEW_H
#define QDROPABLEWEBVIEW_H
#include <QWebEngineView>
#include <memory>

class WebEventListener;
class QWebChannel;
class QWebSocketServer;
class WebSocketClientWrapper;

class DropableWebView : public QWebEngineView
{
    Q_OBJECT
public:
    DropableWebView(QWidget* parent=nullptr);

    void setMarker(double lat, double lng, const QString &fileName);
    void goToCoordinates(double north, double east);
    void removeAllMarkers();

signals:
    void fileChanged(const QString& filePath);
    void imageClickedOnMap(const QString& filePath);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void dragMoveEvent(QDragMoveEvent * event) override;

private:
    std::unique_ptr<WebEventListener> mEventListener;
    QWebChannel* mWebChannel;
    QWebSocketServer* mSocketServer;
    WebSocketClientWrapper* mClientWrapper;
};

#endif // QDROPABLEWEBVIEW_H
