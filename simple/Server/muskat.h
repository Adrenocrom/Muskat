#ifndef MUSKAT_H
#define MUSKAT_H

#pragma once

// STL
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <bitset>

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
#include <QComboBox>
#include <QPushButton>
#include <QRadioButton>
#include <QLineEdit>
#include <QLabel>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMatrix4x4>
#include <QVector3D>
#include <QImage>

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

string insertZeros(uint num_zeros, uint n);

// PRO
#include "fileRenderer.h"
#include "config.h"
#include "scene.h"
#include "playlist.h"
#include "mainwindow.h"
#include "compressor.h"
#include "jsonrpc.h"
#include "deamon.h"

#endif
