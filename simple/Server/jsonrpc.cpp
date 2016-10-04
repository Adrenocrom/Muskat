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
	else if(request.method == "saveFrame") {
		response = saveFrame(request);
	}
	else if(request.method == "newMessure") {
		response = newMessure(request);
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

	int scene_id = request.params["id"].toInt();
	m_mainWindow->m_filerenderer->setScene(&m_mainWindow->m_playlist->m_scenes[scene_id]);
	m_mainWindow->m_evaluator->setScene(&m_mainWindow->m_playlist->m_scenes[scene_id]);

	return response;
}

RPCResponse JsonRPC::getConfig(RPCRequest& request) {
	RPCResponse response;

	return response;
}

RPCResponse JsonRPC::setConfig(RPCRequest& request) {
	RPCResponse response;

	m_mainWindow->m_config->setConfig(request.params);

	return response;
}

RPCResponse JsonRPC::getFrame(RPCRequest& request) {
	RPCResponse response;
	uint id = request.params["id"].toInt();
	
	FrameInfo info = m_mainWindow->m_filerenderer->getScene()->m_infos[id];
	FrameBuffer fb;

	m_mainWindow->m_filerenderer->getFrame(info, fb);
	
	response.result = m_mainWindow->m_compressor->compressFrame(info, fb);

	return response;
}

RPCResponse JsonRPC::saveFrame(RPCRequest& request) {
	RPCResponse response;
	QByteArray base64Data;
	QImage image;

	int frame_id 		= request.params["id"].toInt();
	QString rgb	 		= request.params["rgb"].toString();
	double	duration 	= request.params["time"].toDouble();
	
	rgb.replace(0, 22, "");	// delete data:image/png;base64,
	base64Data.append(rgb);


	image.loadFromData(QByteArray::fromBase64(base64Data), "PNG");
    cv::Mat img(image.height(), image.width(), CV_8UC4, image.bits());

	cv::Mat dst;
	cv::cvtColor(img, dst, CV_BGRA2BGR);
	
	m_mainWindow->m_evaluator->addResult(frame_id, &dst, duration);

	if(m_mainWindow->m_evaluator->hasResults()) {
		m_mainWindow->m_evaluator->runEvaluation("results");
	}


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

RPCResponse JsonRPC::newMessure(RPCRequest& request) {
	RPCResponse response;

	m_mainWindow->m_evaluator->newMessure();

	QJsonObject jo;
	jo["newMessureReady"] = true;
	response.result = jo;

	return response;
}
