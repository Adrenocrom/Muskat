#include "muskat.h"

JsonRPC::JsonRPC(MainWindow *mw) {
    m_mainWindow = mw;
}

JsonRPC::~JsonRPC() {

}

void JsonRPC::parseMessage(QWebSocket* client, QString msg) {
    QJsonDocument jsonRequest = QJsonDocument::fromJson(msg.toUtf8());
    QJsonObject reqObj = jsonRequest.object();
    RPCMessage rpcMessage;

    rpcMessage.jsonrpc = reqObj["jsonrpc"].toString();
    rpcMessage.method  = reqObj["method"].toString();
    rpcMessage.params  = reqObj["params"].toObject();
    rpcMessage.id      = reqObj["id"].toInt();

    if(rpcMessage.method == "resize") {
        std::cout<<"r"<<std::endl;
        int w = rpcMessage.params["width"].toInt();
        int h = rpcMessage.params["height"].toInt();
        m_mainWindow->m_glWidget->setMinimumSize(w, h);;
        m_mainWindow->m_glWidget->setMaximumSize(w, h);

        m_mainWindow->m_qle_widget_width_config->setText(QString::number(m_mainWindow->m_glWidget->width()));
        m_mainWindow->m_qle_widget_height_config->setText(QString::number(m_mainWindow->m_glWidget->height()));
    }
    else if(rpcMessage.method == "getFrame") {
        SBuffers buffer = m_mainWindow->m_glWidget->renderFrame();
        QJsonObject bufferObj;
        bufferObj["rgb"] = QString(QByteArray((const char *)buffer.rgb).toBase64());

        QJsonObject resObj;
        resObj["jsonrpc"] = "2.0";
        resObj["result"] = bufferObj;

        QJsonDocument jsonResponse(resObj);
        client->sendTextMessage(jsonResponse.toJson(QJsonDocument::Compact));

        delete[] buffer.rgb;
    }
}
