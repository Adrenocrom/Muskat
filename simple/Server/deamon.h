/***********************************************************
 *
 *
 *						DEAMON HEADER
 *					 ===================
 *
 *		AUTHOR: Josef Schulz
 *
 *		Implements the WebSocket server, based on the
 *		echosever example from the qt framework
 *
 ***********************************************************/

#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#pragma once

#include <QObject>

class QWebSocketServer;
class QWebSocket;
class JsonRPC;
class MainWindow;

class ServerDeamon : public QObject
{
    Q_OBJECT
public:
    explicit ServerDeamon(MainWindow* mainWindow, quint16 port, QObject *parent = Q_NULLPTR);
    ~ServerDeamon();

Q_SIGNALS:
    void closed();

private Q_SLOTS:

	// handles what happend, when is connected
    void onNewConnection();

	// a recevied messeage will be handled, therefor the message will be parsed with jsonrpc
    void processTextMessage(QString message);		
	
	// handles binary message not used here, but it sould ;)
    void processBinaryMessage(QByteArray message);

	// remove the client from the client list
    void socketDisconnected();

private:
    QWebSocketServer*	m_pWebSocketServer;	// handles the websocket protokoll
    QList<QWebSocket*> 	m_clients;			// list of conneced client
    MainWindow*       	m_pMainWindow;

	// stores the jsonrpc session, for every client a own session
	// should be initiated, but this step is not implemented
    JsonRPC*          	m_pJsonRPC;			
};

#endif //ECHOSERVER_H
