#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#pragma once

#include "muskat.h"

class QWebSocketServer;
class QWebSocket;

class ServerDeamon : public QObject
{
    Q_OBJECT
public:
    explicit ServerDeamon(MainWindow* mainWindow, quint16 port, bool debug = false, QObject *parent = Q_NULLPTR);
    ~ServerDeamon();

Q_SIGNALS:
    void closed();

private Q_SLOTS:
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();

private:
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
    MainWindow*       m_mainWindow;
    bool m_debug;
};

#endif //ECHOSERVER_H
