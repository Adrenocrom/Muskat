#include "muskat.h"

JsonRPC::JsonRPC(MainWindow *mw) {
    m_mainWindow = mw;
    m_buffer = NULL;
    m_buffer = new SBuffer();
    m_buffer->rgb = new GLubyte[m_mainWindow->m_glWidget->width()*
                                m_mainWindow->m_glWidget->height()*3];
}

JsonRPC::~JsonRPC() {
    if(m_buffer) {
        delete m_buffer;
        m_buffer = NULL;
    }
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
        m_mainWindow->m_glWidget->setMinimumSize(w, h);
        m_mainWindow->m_glWidget->setMaximumSize(w, h);
        m_mainWindow->m_qle_widget_width_config->setText(QString::number(m_mainWindow->m_glWidget->width()));
        m_mainWindow->m_qle_widget_height_config->setText(QString::number(m_mainWindow->m_glWidget->height()));

        m_buffer->delete_buffer();
        m_buffer->rgb = new GLubyte[m_mainWindow->m_glWidget->width() * m_mainWindow->m_glWidget->height() * 3];
        //m_buffer->d   = new GLfloat[w * h];
    }
    else if(rpcMessage.method == "getFrame") {
        std::cout<<"Message Id: "<<rpcMessage.id<<std::endl;
        m_mainWindow->m_glWidget->renderFrame(m_buffer);

        const int JPEG_QUALITY = 1; // 1 = worst, 100 = best
            //const int COLOR_COMPONENTS = 3;
        long unsigned int _jpegSize = 0;
        unsigned char* _compressedImage = NULL;

        tjhandle _jpegCompressor = tjInitCompress();

        tjCompress2(_jpegCompressor,
                    m_buffer->rgb,
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

        QByteArray test;
        test.append((const char*)_compressedImage, _jpegSize);

        QJsonObject bufferObj;
        bufferObj["rgb"] = QString(test.toBase64());
        tjFree(_compressedImage);

        bufferObj["size"] = (int)sizeof(m_buffer->rgb);

        QJsonObject resObj;
        resObj["jsonrpc"] = "2.0";
        resObj["result"] = bufferObj;

        QJsonDocument jsonResponse(resObj);
        client->sendTextMessage(jsonResponse.toJson(QJsonDocument::Compact));

    }
}
