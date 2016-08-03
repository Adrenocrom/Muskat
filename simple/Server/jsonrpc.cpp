#include "muskat.h"

JsonRPC::JsonRPC(MainWindow *mw) {
    m_mainWindow 	= mw;
	m_msg_counter 	= 0;
}

JsonRPC::~JsonRPC() {
}

void JsonRPC::parseMessage(QWebSocket* client, QString msg) {
    QJsonDocument jsonRequest = QJsonDocument::fromJson(msg.toUtf8());
    QJsonObject reqObj = jsonRequest.object();
    RPCRequest 	request;
	RPCResponse response;

    request.jsonrpc = reqObj["jsonrpc"].toString();
    request.method  = reqObj["method"].toString();
    request.params  = reqObj["params"].toObject();
    request.id      = reqObj["id"].toInt();

	if(request.method == "getPlaylist") {
		response = getPlaylist(request);
	}
	else if(request.method == "loadScene") {
		response = loadScene(request);
	}
	else if(request.method == "getConig") {
		response = getConfig(request);
	
	}
	else if(request.method == "setConfig") {
		response = setConfig(request);
	}
	else if(request.method == "getFrame") {
		response = getFrame(request);
	}

	response.id 	 = request.id;
	response.jsonrpc = "2.0";

	QJsonObject resObj;
	resObj["jsonrpc"] 	= response.jsonrpc;
	resObj["result"] 	= response.result;
	resObj["id"]		= response.id;

	QJsonDocument jsonResponse = QJsonDocument(resObj);
	client->sendTextMessage(QString(jsonResponse.toJson(QJsonDocument::Compact)));
}

RPCResponse JsonRPC::getPlaylist(RPCRequest& request) {
	RPCResponse response;

	response.result = m_mainWindow->m_playlist->getPlaylist();
	
	return response;
}

RPCResponse JsonRPC::loadScene(RPCRequest& request) {
	RPCResponse response;

	//QString scene_name = request.params["name"].toString();
	m_mainWindow->m_filerenderer->setScene(&m_mainWindow->m_playlist->m_scenes[0]);

	return response;
}

RPCResponse JsonRPC::getConfig(RPCRequest& request) {
	RPCResponse response;

	return response;
}

RPCResponse JsonRPC::setConfig(RPCRequest& request) {
	RPCResponse response;

	return response;
}

RPCResponse JsonRPC::getFrame(RPCRequest& request) {
	RPCResponse response;
	uint id = request.params["id"].toInt();
	
	SFrameInfo info;
	info.id = id;
	SFrameBuffer fb;

	m_mainWindow->m_filerenderer->getFrame(info, fb);
	
	response.result = Compressor::compressFrame(info, fb);

	return response;
}

RPCResponse JsonRPC::resize(RPCRequest& request) {
	RPCResponse response;

	QJsonObject jo;
	jo["width"] 	= 512;
	jo["height"]	= 512;
	response.result = jo;

	return response;
}
