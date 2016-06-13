QT += core gui widgets websockets opengl

TARGET   = muskat
TEMPLATE = app

SOURCES += main.cpp \
           muskat.cpp \
           glwidget.cpp \
           mainwindow.cpp \
           deamon.cpp

HEADERS += muskat.h \
           glwidget.h \
           mainwindow.h \
           deamon.h

RESOURCES += shaders.qrc \
             muskat.qrc
