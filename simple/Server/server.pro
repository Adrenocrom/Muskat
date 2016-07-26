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

LIBS += -L/home/josef/Programme/Qt5.6.0/5.6/gcc_64/lib \
        -L/usr/lib/x86_64-linux-gnu/ -lturbojpeg

INCLUDEPATH += /usr/include/opencv2/
