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

RESOURCES += shaders.qrc \
             muskat.qrc
