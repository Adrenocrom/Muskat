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
        long unsigned int _jpegSize = 0;
        unsigned char* _compressedImage = NULL;

        std::cout<<"a"<<std::endl;
        tjhandle _jpegCompressor = tjInitCompress();

        std::cout<<"b"<<std::endl;
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

        std::cout<<"c"<<std::endl;
        tjDestroy(_jpegCompressor);

        std::cout<<"d"<<std::endl;
        QByteArray test;
        std::cout<<"e"<<std::endl;
        test.append((const char*)_compressedImage, _jpegSize);

        std::cout<<"f "<<_jpegSize<<std::endl;
        QJsonObject bufferObj;

        std::cout<<"g "<<test.size()<<std::endl;
        QString str = QString(test.toBase64());

        std::cout<<"h"<<std::endl;
        bufferObj["rgb"] = str;

        std::cout<<"i"<<std::endl;
        tjFree(_compressedImage);



        //bufferObj["size"] = (int)_jpegSize;

        std::cout<<"j"<<std::endl;
        QJsonObject resObj;

        std::cout<<"k"<<std::endl;
        resObj["jsonrpc"] = "2.0";

        std::cout<<"l"<<std::endl;
        resObj["result"] = bufferObj;

        std::cout<<"m"<<std::endl;
        QJsonDocument jsonResponse(resObj);

        std::cout<<"n"<<std::endl;
        client->sendTextMessage(jsonResponse.toJson(QJsonDocument::Compact));

    }
}
