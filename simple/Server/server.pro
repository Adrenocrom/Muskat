QT += core gui widgets websockets opengl

TARGET = server
TEMPLATE = app

SOURCES += main.cpp \
			  muskat.cpp \
			  deamon.cpp \
			  jsonrpc.cpp \
			  abstractRenderer.cpp \
			  fileRenderer.cpp \
			  mainwindow.cpp 

HEADERS += muskat.h \
			  deamon.h \
			  jsonrpc.h \
			  abstractRenderer.h \
			  fileRenderer.h \
			  mainwindow.h 

LIBS += -lboost_system \
		  -lboost_filesystem

INCLUDEPATH += /usr/include/opencv2/ \
					/usr/include/boost/

RESOURCES += server.qrc
