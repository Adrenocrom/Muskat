#include "muskat.h"

QT_USE_NAMESPACE

ServerDeamon::ServerDeamon(MainWindow* mainWindow, quint16 port, bool debug, QObject *parent) : QObject(parent) {
	m_pMainWindow = mainWindow;
	m_pWebSocketServer = new QWebSocketServer(QStringLiteral("Server"), QWebSocketServer::NonSecureMode, this);
	m_debug = debug;

	if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &ServerDeamon::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &ServerDeamon::closed);
   }

   m_pJsonRPC = nullptr;
   m_pJsonRPC = new JsonRPC(m_pMainWindow);
}

ServerDeamon::~ServerDeamon() {
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
	
	SAFE_DELETE(m_pJsonRPC);
}

void ServerDeamon::onNewConnection() {
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &ServerDeamon::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &ServerDeamon::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &ServerDeamon::socketDisconnected);

    m_clients << pSocket;
}

void ServerDeamon::processTextMessage(QString message) {
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (m_debug)
        qDebug() << "Message received:" << message;
    if(pClient) {
        m_pJsonRPC->parseMessage(pClient, message);
    }
}

void ServerDeamon::processBinaryMessage(QByteArray message) {
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (m_debug)
        qDebug() << "Binary Message received:" << message;
    if (pClient) {
        pClient->sendBinaryMessage(message);
    }
}

void ServerDeamon::socketDisconnected() {
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (m_debug)
        qDebug() << "socketDisconnected:" << pClient;
    if (pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}
