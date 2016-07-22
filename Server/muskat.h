#ifndef MUSKAT_H
#define MUSKAT_H

#pragma once

// standart includes
#include <iostream>
#include <math.h>

// qt includes
#include <QApplication>
#include <QSurfaceFormat>
#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>
#include <QGridLayout>
#include <QStatusBar>
#include <QFormLayout>
#include <QDockWidget>
#include <QMenuBar>
#include <QGroupBox>
#include <QSlider>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QScrollArea>
#include <QSpacerItem>
#include <QPainter>
#include <QPaintEngine>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QVector3D>
#include <QVector2D>
#include <QMatrix4x4>
#include <QTime>
#include <QVector>
#include <QList>
#include <QCoreApplication>
#include <QtWebSockets/qwebsocketserver.h>
#include <QtWebSockets/qwebsocket.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtCore>
#include <turbojpeg.h>

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p != nullptr) { delete (p); (p) = nullptr; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p != nullptr) { delete[] (p); (p) = nullptr; } }
#endif

// project includes
#include "mainwindow.h"
#include "glwidget.h"
#include "deamon.h"
#include "jsonrpc.h"
#include "scene.h"

#endif
