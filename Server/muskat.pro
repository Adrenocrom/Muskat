QT += core gui widgets websockets opengl

TARGET   = muskat
TEMPLATE = app

SOURCES += main.cpp \
           muskat.cpp \
           glwidget.cpp \
           mainwindow.cpp \
           deamon.cpp \
           jsonrpc.cpp \

HEADERS += muskat.h \
           glwidget.h \
           mainwindow.h \
           deamon.h \
           jsonrpc.h

LIBS    += -L/home/josef/Programme/Qt5.6.0/5.6/gcc_64/lib \
           -L/usr/lib/x86_64-linux-gnu/ -lturbojpeg

RESOURCES += shaders.qrc \
             muskat.qrc
