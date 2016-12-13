/***********************************************************
 *
 *
 *						 MAIN SOURCE
 *					 ===================
 *
 *		AUTHOR: Josef Schulz
 *
 *		This file creates the MainWindow.
 *		All files are included in muskat.h.
 *		And the MainWindow is defined in mainwindow.h
 *
 *
 *
 ***********************************************************/

#include "muskat.h"

int main(int argc, char** argv) {
	QApplication app(argc, argv);

	MainWindow mw;
	mw.show();

	return app.exec();
}
