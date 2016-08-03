QT += core gui widgets websockets opengl

TARGET = server
TEMPLATE = app

SOURCES +=	main.cpp \
			muskat.cpp \
			deamon.cpp \
			scene.cpp \
			playlist.cpp \
			config.cpp \
			compressor.cpp \
			jsonrpc.cpp \
			fileRenderer.cpp \
			mainwindow.cpp 

HEADERS += 	muskat.h \
			deamon.h \
			scene.h \
			playlist.h \
			config.h \
			compressor.h \
			jsonrpc.h \
			fileRenderer.h \
			mainwindow.h 

LIBS += -lboost_system \
		-lboost_filesystem \
		-lopencv_core \
		-lopencv_highgui

INCLUDEPATH += 	/usr/include/opencv2/ \
				/usr/include/boost/

RESOURCES += server.qrc
