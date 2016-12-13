/***********************************************************
 *
 *
 *						JSONRPC SOURCE
 *					 ====================
 *
 *		AUTHOR: Josef Schulz
 *
 *		Defines the jsonrpc class. More informations
 *		are in jsonrpc.h
 *		Validation should be added for usage in public
 *		networks.
 *
 ***********************************************************/


#include "muskat.h"

JsonRPC::JsonRPC(MainWindow *mw) {
    m_mainWindow 	= mw;
	m_msg_counter 	= 0;
}

JsonRPC::~JsonRPC() {
}

// wrapper function to handle the normal stuff like
// the jsonrpc string and id
// for every possible method a seperate function exists
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
		response = getPlaylist();
	}
	else if(request.method == "loadScene") {
		response = loadScene(request);
	}
	else if(request.method == "getConig") {
		response = getConfig();
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

	// create jsondocument from object and send the response to the client
	QJsonDocument jsonResponse = QJsonDocument(resObj);
	client->sendTextMessage(QString(jsonResponse.toJson(QJsonDocument::Compact)));
}

// sends playlist to the client
RPCResponse JsonRPC::getPlaylist() {
	RPCResponse response;

	response.result = m_mainWindow->m_playlist->getPlaylist();
	
	return response;
}

// used to set a scene from playlist as active,
// is required if the server is not able to handle more than one big
// scene at one time, therefor its used here too.
RPCResponse JsonRPC::loadScene(RPCRequest& request) {
	RPCResponse response;

	int scene_id = request.params["id"].toInt();
	m_mainWindow->m_filerenderer->setScene(&m_mainWindow->m_playlist->m_scenes[scene_id]);
	m_mainWindow->m_evaluator->setScene(&m_mainWindow->m_playlist->m_scenes[scene_id]);

	QJsonObject jo;
	jo["sceneReady"] = true;
	response.result = jo;
	return response;
}

// not implemented just a dummy method
RPCResponse JsonRPC::getConfig() {
	RPCResponse response;

	return response;
}

// overrides the config of the server and send the success
// to the client
RPCResponse JsonRPC::setConfig(RPCRequest& request) {
	RPCResponse response;

	m_mainWindow->m_config->setConfig(request.params);

	QJsonObject jo;
	jo["configReady"] = true;
	response.result = jo;
	return response;
}

// compress the frame identified by the frameid.
// alternative the info can be sendet by the 
// client to initiate the render process
RPCResponse JsonRPC::getFrame(RPCRequest& request) {
	RPCResponse response;
	uint id = request.params["id"].toInt();
	
	// load info by id
	FrameInfo info = m_mainWindow->m_filerenderer->getScene()->m_infos[id];
	FrameBuffer fb;

	// at this place an real renderer could be insert
	m_mainWindow->m_filerenderer->getFrame(info, fb);
	
	// compress the rgb and the depth image,
	// the params are stored in the config
	response.result = m_mainWindow->m_compressor->compressFrame(info, fb);

	return response;
}

// this method will be used to save the extrapolated frames
// and evaluate them with the loaded scenes as ground truth
RPCResponse JsonRPC::saveFrame(RPCRequest& request) {
	RPCResponse response;
	QByteArray base64Data;
	QImage image;

	int frame_id 		= request.params["id"].toInt();
	QString rgb	 		= request.params["rgb"].toString();
	double	duration 	= request.params["time"].toDouble();
	
	rgb.replace(0, 22, "");	// delete data:image/png;base64,
	base64Data.append(rgb);

	// creates an cv::Mat image from base64 string
	image.loadFromData(QByteArray::fromBase64(base64Data), "PNG");
    cv::Mat img(image.height(), image.width(), CV_8UC4, image.bits());

	cv::Mat dst;
	cv::cvtColor(img, dst, CV_BGRA2BGR); // OpenCv ;)
	
	// add recived image to the list, add results increments a counter
	// and if alls frames of the scene are recived, hasResult will be true
	m_mainWindow->m_evaluator->addResult(frame_id, &dst, duration);

	// if all frames are recived the ssim and psnr can be calculated
	// and after that an response will be sendet to inform the client
	if(m_mainWindow->m_evaluator->hasResults()) {
		m_mainWindow->m_evaluator->runEvaluation();
		
		QJsonObject jo;
		jo["messureReady"] = m_mainWindow->m_evaluator->getEvaluationId();
		response.result = jo;
	}


	return response;
}

// is also an dummy function not needed to messure
// the ground truth scenes, but could be helpfull to handle
// a real system
RPCResponse JsonRPC::resize() {
	RPCResponse response;

	QJsonObject jo;
	jo["width"] 	= 512;
	jo["height"]	= 512;
	response.result = jo;

	return response;
}

// Clients informs the server to wait for extrapolated frames.
// This message is needed to resset the counter.
RPCResponse JsonRPC::newMessure(RPCRequest& request) {
	RPCResponse response;

	int	scene_id 		= request.params["sceneId"].toInt();
	int messure_id		= request.params["messureId"].toInt();
	QString name 		= request.params["name"].toString();
	QString short_name 	= request.params["shortName"].toString();

	// one mesh for all extraolated images, information about
	// the already exits
	int num_vertices	= request.params["numVertices"].toInt();
	int	num_indices 	= request.params["numIndices"].toInt();
	int num_triangles	= request.params["numTriangles"].toInt();
	
	m_mainWindow->m_evaluator->newMessure(scene_id,
										  messure_id, 
										  name.toStdString(),
										  short_name.toStdString(),
										  num_vertices,
										  num_indices,
										  num_triangles);

	QJsonObject jo;
	jo["newMessureReady"] = true;
	response.result = jo;

	return response;
}
