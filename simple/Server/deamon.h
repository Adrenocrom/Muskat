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
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();

private:
    QWebSocketServer*	m_pWebSocketServer;
    QList<QWebSocket*> 	m_clients;
    MainWindow*       	m_pMainWindow;
    JsonRPC*          	m_pJsonRPC;
};

#endif //ECHOSERVER_H
