/***********************************************************
 *
 *
 *						DEAMON SOURCE
 *					 ===================
 *
 *		AUTHOR: Josef Schulz
 *
 *		Definition of the server deamon.
 *		More informations in deamon.h
 *
 ***********************************************************/

#include "muskat.h"

QT_USE_NAMESPACE

// starts the server (not in secure mode), and initiate jsonrpc 
ServerDeamon::ServerDeamon(MainWindow* mainWindow, quint16 port, QObject *parent) : QObject(parent) {
	m_pMainWindow = mainWindow;
	m_pWebSocketServer = new QWebSocketServer(QStringLiteral("Server"), QWebSocketServer::NonSecureMode, this);

	if (m_pWebSocketServer->listen(QHostAddress::Any, port)) {
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection, this, &ServerDeamon::onNewConnection);
        connect(m_pWebSocketServer, &QWebSocketServer::closed, this, &ServerDeamon::closed);
   }

   m_pJsonRPC = nullptr;
   m_pJsonRPC = new JsonRPC(m_pMainWindow);
}

// shutdown the server, deletes all clients from the list
// and delte jsonrpc
ServerDeamon::~ServerDeamon() {
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
	
	SAFE_DELETE(m_pJsonRPC);
}

// add a new client to the client list
void ServerDeamon::onNewConnection() {
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &ServerDeamon::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &ServerDeamon::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &ServerDeamon::socketDisconnected);

    m_clients << pSocket;
}

// handle the recived message with jsonrpc
void ServerDeamon::processTextMessage(QString message) {
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if(pClient) {
        m_pJsonRPC->parseMessage(pClient, message);
    }
}

// if a binary message will be recived response with the recived message
void ServerDeamon::processBinaryMessage(QByteArray message) {
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        pClient->sendBinaryMessage(message);
    }
}

// remove the client from when it disconneced
void ServerDeamon::socketDisconnected() {
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
	if (pClient) {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}
