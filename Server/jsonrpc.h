#ifndef JSONRPC_H
#define JSONRPC_H

#pragma once

struct SBuffer;

struct RPCMessage {
    QString     jsonrpc;
    QString     method;
    QJsonObject params;
    int         id;
};

class JsonRPC {
public:
    JsonRPC(MainWindow* mw);
    ~JsonRPC();

    void parseMessage(QWebSocket* client, QString msg);
private:
    MainWindow* m_mainWindow;
  //  QMutex      m_mutex;

    SBuffer*    m_buffer;
};

#endif
