#ifndef GLWIDGET_H
#define GLWIDGET_H

#pragma once

#include "muskat.h"

class MainWindow;

struct SBuffer {
    GLubyte* rgb;
    GLfloat* d;
};

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    GLWidget(MainWindow *mw);
    ~GLWidget();

    void renderFrame(SBuffer* buffer);
    void resize(int w, int h);

protected:
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void initializeGL() Q_DECL_OVERRIDE;
    void timerEvent(QTimerEvent *e) Q_DECL_OVERRIDE;

    void initShaders();
    void initTextures();

private:
    void initScene();
    void drawScene();
    void exitScene();

    QBasicTimer timer;
    MainWindow *m_mainWindow;
    QOpenGLShaderProgram* program;

    QMatrix4x4 projection;
    QMatrix4x4 camera;

    QOpenGLTexture* texture;
    QOpenGLBuffer arrayBuf;
    QOpenGLBuffer indexBuf;
};

#endif
