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
#include <QCoreApplication>
#include <QThread>
#include <QMessageBox>

DropableWebView::DropableWebView(QWidget *parent)
    : QWebEngineView(parent)
    , mRequestId(0)
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
    //get the coordinates of the click position from the map (via the java script)
    //ad assign these coordinates to the dropped files
    auto fileNames = event->mimeData()->text().split("|");
    mEventListener->scheduleFiles(++mRequestId, fileNames);
    QString str = QStringLiteral("getLocationAtMousePosition(%1,%2,\"%3\");").arg(event->posF().x()).arg(event->posF().y()).arg(mRequestId);
    page()->runJavaScript(str);
}

void DropableWebView::dragMoveEvent(QDragMoveEvent * event)
{
    event->acceptProposedAction();
}

void DropableWebView::waitForFinished()
{
    if(!mEventListener->isFinished())
    {
        QMessageBox box(this);
        box.setText("Please wait.\nSome fotos are still being tagged in the bakground.");//, QMessageBox::NoButton);
        box.setStandardButtons(QMessageBox::NoButton);
        box.show();
        while(!mEventListener->isFinished())
        {
            QThread::msleep(100);
            qApp->processEvents();
        }
    }
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
