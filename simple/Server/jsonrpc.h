#ifndef JSONRPC_H
#define JSONRPC_H

#pragma once

struct RPCRequest {
    QString     jsonrpc;
    QString     method;
    QJsonObject params;
    int         id;
};

struct RPCResponse {
	QString 	jsonrpc;
	QJsonObject result;
	int 		id;
};

class JsonRPC {
public:
	uint 		m_msg_counter;
	Config 		m_config;
	Compressor	m_compressor;

    JsonRPC(MainWindow* mw);
    ~JsonRPC();

    void parseMessage(QWebSocket* client, QString msg);

	RPCResponse getPlaylist(RPCRequest& request);
	RPCResponse loadScene(RPCRequest& request);
	RPCResponse getConfig(RPCRequest& request);
	RPCResponse setConfig(RPCRequest& request);
	RPCResponse getFrame(RPCRequest& request);
	RPCResponse resize(RPCRequest& request);

private:
    MainWindow* m_mainWindow;

};

#endif
