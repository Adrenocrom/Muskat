#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include <QMainWindow>

class QStackedWidget;
class QGroupBox;
class QPushButton;
class QLineEdit;
class QComboBox;
class QLabel;

class FileRenderer;
class ServerDeamon;
class Playlist;

/*
namespace boost {
	namespace filesystem {
		class path;
	};
};*/

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	ServerDeamon*				m_serverDeamon;
	FileRenderer*				m_filerenderer;

	Playlist*					m_playlist;

	MainWindow();
	~MainWindow();
	
	void resizeEvent(QResizeEvent*);

private slots:

	void start_server();
	void stop_server();

private:
	int				m_server_port;
	std::string		m_scenes_dir;
	std::string		m_scene_suffix;

	QStackedWidget*	m_widget_stacked;
	QWidget*		m_widget_start;
	QWidget*		m_widget_main;

	QAction*		m_action_start_server;
	QAction*		m_action_stop_server;

	QPushButton*	m_button_start_server;
	QLineEdit*		m_lineEdit_server_port;
	QLineEdit*		m_lineEdit_scene_suffix;
	QLineEdit*		m_lineEdit_scenes_dir;

	void 		createWidgetStart();
	void 		createWidgetMain();
	QGroupBox* 	createWidgetTransport();
	QGroupBox* 	createWidgetScene();
	void 		createMenu();
};

#endif
