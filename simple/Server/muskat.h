/***********************************************************
 *
 *
 *						MUSKAT HEADER
 *					 ===================
 *
 *		AUTHOR: Josef Schulz
 *
 *		This file includes all required libraries.
 *		Off the STL, OpenCV, Boost, Qt and the
 *		project itself.
 *		
 *		Some definitions and functions are provided by
 *		this file.
 *
 *
 *
 ***********************************************************/

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
#include <set>
#include <unordered_set>
#include <map>
#include <ctime>

// CV
#include <opencv.hpp>
#include <opencv2/gpu/gpu.hpp>

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
#include <QVariant>
#include <QList>
#include <QProgressBar>

// DEFINES
#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p != nullptr) { delete (p); (p) = nullptr; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p != nullptr) { delete[] (p); (p) = nullptr; } }
#endif

#ifndef MU_MIN
#define MU_MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MU_MAX
#define MU_MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

// wrapper to find the minimum of 4 values
#ifndef MU_MIN4	
#define MU_MIN4(a, b, c, d) (MU_MIN(a, MU_MIN(b, MU_MIN(c, d))))
#endif

// wrapper to find the maximum of 4 values
#ifndef MU_MAX4
#define MU_MAX4(a, b, c, d) (MU_MAX(a, MU_MAX(b, MU_MAX(c, d))))
#endif


typedef unsigned int uint;
typedef unsigned short ushort;

using namespace std;

// creates a string of a value and creates leading zeroes
string insertZeros(uint num_zeros, uint n);

// Program
#include "fileRenderer.h"	// this is a demo class, for an arbitrary renderer
#include "config.h"			// stores the config information
#include "scene.h"			// scene, consits of n triples (rgb, depth, camera infos)
#include "playlist.h"		// playlist consits of scenes
#include "mainwindow.h"		// widget class, for gui handling. Also defines main components
#include "quadtree.h"		// quadtree implementation
#include "compressor.h"		// class, which handles the hole compression part
#include "jsonrpc.h"		// lightweight jsonrpc implementation
#include "deamon.h"			// server deamon, handles the connection 
#include "evaluator.h"		// this class implements the evaluation elements like SSIM, PSNR and more

#endif
