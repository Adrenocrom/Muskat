#include "muskat.h"

JsonRPC::JsonRPC(MainWindow *mw) {
    m_mainWindow = mw;

    rgb.resize(m_mainWindow->m_glWidget->width() * m_mainWindow->m_glWidget->height() * 3);
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

        int w = rpcMessage.params["width"].toInt();
        int h = rpcMessage.params["height"].toInt();
        m_mainWindow->m_glWidget->setMinimumSize(w, h);
        m_mainWindow->m_glWidget->setMaximumSize(w, h);
        m_mainWindow->m_qle_widget_width_config->setText(QString::number(w));
        m_mainWindow->m_qle_widget_height_config->setText(QString::number(h));

        rgb.resize(w * h * 3);
    }
    else if(rpcMessage.method == "getFrame") {
        //std::cout<<"Message Id: "<<rpcMessage.id<<std::endl;
        SBuffer buffer;
        buffer.rgb = rgb.data();
        m_mainWindow->m_glWidget->renderFrame(&buffer);

        const int JPEG_QUALITY = 1; // 1 = worst, 100 = best
        long unsigned int _jpegSize = 0;
        unsigned char* _compressedImage = NULL;

        tjhandle _jpegCompressor = tjInitCompress();

        tjCompress2(_jpegCompressor,
                    buffer.rgb,
                    m_mainWindow->m_glWidget->width(),
                    0,
                    m_mainWindow->m_glWidget->height(),
                    TJPF_RGB,
                    &_compressedImage,
                    &_jpegSize,
                    TJSAMP_444,
                    JPEG_QUALITY,
                    TJFLAG_FASTDCT);

        tjDestroy(_jpegCompressor);

        QByteArray bytes;
        bytes.append((const char*)_compressedImage, _jpegSize);

        QJsonObject buffer_obj;
        buffer_obj["rgb"] = QString(bytes.toBase64());
        buffer_obj["size"] = (int)_jpegSize;

        QJsonObject res_obj;
        res_obj["jsonrpc"] = "2.0";
        res_obj["result"] =  buffer_obj;

        QJsonDocument jsonResponse = QJsonDocument(res_obj);

        client->sendTextMessage(QString(jsonResponse.toJson(QJsonDocument::Compact)));

        tjFree(_compressedImage);
    }
}
