#include "dropablewebview.h"
#include "webeventlistener.h"
#include "websockettransport.h"
#include "websocketclientwrapper.h"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QWebChannel>
#include <QWebSocketServer>

DropableWebView::DropableWebView(QWidget *parent)
    : QWebEngineView(parent)
{
    setAcceptDrops(true);

    // setup the QWebSocketServer
    mSocketServer = new QWebSocketServer(QStringLiteral("QWebChannel Server"), QWebSocketServer::NonSecureMode);
    if (!mSocketServer->listen(QHostAddress::LocalHost, 12345)) {
        qFatal("Failed to open web socket server.");
        return;
    }

    // wrap WebSocket clients in QWebChannelAbstractTransport objects
    mClientWrapper = new WebSocketClientWrapper(mSocketServer);

    // setup the channel
    mWebChannel = new QWebChannel();
    QObject::connect(mClientWrapper, &WebSocketClientWrapper::clientConnected, mWebChannel, &QWebChannel::connectTo);

    mEventListener = std::make_unique<WebEventListener>(new WebEventListener());
    connect(mEventListener.get(), &WebEventListener::fileChanged, this, &DropableWebView::fileChanged);
    connect(mEventListener.get(), &WebEventListener::markerClicked, this, &DropableWebView::imageClickedOnMap);

    mWebChannel->registerObject("listener", mEventListener.get());
}

void DropableWebView::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void DropableWebView::dropEvent(QDropEvent *event)
{
    QWebEngineView::dropEvent(event);

    auto fileNames = event->mimeData()->text().split("|");
    for(auto file : fileNames)
    {
        qDebug() << file;
        QString str = QStringLiteral("getLocationAtMousePosition(%1,%2,\"%3\");").arg(event->posF().x()).arg(event->posF().y()).arg(file);
        page()->runJavaScript(str);
    }
}

void DropableWebView::dragMoveEvent(QDragMoveEvent * event)
{
    event->acceptProposedAction();
}

void DropableWebView::goToCoordinates(double lat, double lng)
{
    QString str = QStringLiteral("goTo(%1,%2);").arg(lat).arg(lng);
    page()->runJavaScript(str);
}

void DropableWebView::setMarker(double lat, double lng, const QString& fileName)
{
    QString str = QStringLiteral("addMarker(%1,%2,\"%3\");").arg(lat).arg(lng).arg(fileName);
    page()->runJavaScript(str);
}

void DropableWebView::removeAllMarkers()
{
    page()->runJavaScript("removeAllMarkers()");
}
