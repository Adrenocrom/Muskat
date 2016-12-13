/***********************************************************
 *
 *
 *						JSONRPC HEADER
 *					 ====================
 *
 *		AUTHOR: Josef Schulz
 *
 *		Lightweighted implementation of JSON-RPC.
 *		Consists of two types of messages:
 *			
 *				RPCRequest
 *				RPCResponse
 *
 *		And a class jsonrpc, which handles all the messages
 *		and handles the usage of server-methods.
 *
 ***********************************************************/


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
	uint 		m_msg_counter;	// counts the messages

    JsonRPC(MainWindow* mw);
    ~JsonRPC();

	// takes an request, runs the method and produce an response
	// this method is used by the sever deamon, when
	// an message recevied
    void parseMessage(QWebSocket* client, QString msg);

	// handles all possible requests
	RPCResponse getPlaylist();
	RPCResponse loadScene(RPCRequest& request);
	RPCResponse getConfig();
	RPCResponse setConfig(RPCRequest& request);
	RPCResponse getFrame(RPCRequest& request);
	RPCResponse saveFrame(RPCRequest& request);
	RPCResponse resize();
	RPCResponse newMessure(RPCRequest& request);

private:
    MainWindow* m_mainWindow;

};

#endif
