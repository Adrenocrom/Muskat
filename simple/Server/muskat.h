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
#include <limits.h>

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

#ifndef MU_MIN
#define MU_MIN(a, b) ((a) < (b) ? (a) : (b))													// Minimum
#endif

#ifndef MU_MAX
#define MU_MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MU_MIN4
#define MU_MIN4(a, b, c, d) (MU_MIN(a, MU_MIN(b, MU_MIN(c, d))))
#endif

#ifndef MU_MAX4
#define MU_MAX4(a, b, c, d) (MU_MAX(a, MU_MAX(b, MU_MAX(c, d))))
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
#include "evaluator.h"

#endif
