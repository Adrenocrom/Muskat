#ifndef MUSKAT_H
#define MUSKAT_H

#pragma once

// STL
#include <iostream>
#include <math.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

// CV
#include <opencv.hpp>

// Boost
#include <filesystem.hpp>
#include <range/iterator_range.hpp>

// QT
#include <QApplication>
#include <QMainWindow>
#include <QStackedWidget>
#include <QMenuBar>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMatrix4x4>
#include <QVector3D>

// DEFINES
#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p != nullptr) { delete (p); (p) = nullptr; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p != nullptr) { delete[] (p); (p) = nullptr; } }
#endif

typedef unsigned int uint;
typedef unsigned short ushort;

using namespace std;

// PRO
#include "abstractRenderer.h"
#include "fileRenderer.h"
#include "mainwindow.h"
#include "jsonrpc.h"
#include "deamon.h"

#endif
