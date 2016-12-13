/***********************************************************
 *
 *
 *						MAINWINDOW HEADER
 *					 =======================
 *
 *		AUTHOR: Josef Schulz
 *
 *		Declaration of the MainWindow class.
 *		Consists of:
 *				
 *				server-deamon 
 *				renderer(filerenderer)
 *				playlist 
 *				config
 *				compressor
 *				evaluator
 *
 *
 ***********************************************************/

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
class QProgressBar;

class FileRenderer;
class ServerDeamon;
class Playlist;
class Config;
class Compressor;
class Evaluator;

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	ServerDeamon*			m_serverDeamon;		// deamon is the actual WebSocket server
												// creates the jsonrpc object

	FileRenderer*			m_filerenderer;		// demo clas, like an abstract renderer

	Playlist*				m_playlist;			// creats a playlist from the scenes
	Config*					m_config;			// holds the config, shard with the client
	Compressor*				m_compressor;		// handles the hole compression part
	Evaluator*				m_evaluator;		// evaluate all values
	
	QProgressBar*			m_progress_load;	// progressbar, visible while loading the scenes

	MainWindow();
	~MainWindow();
	
	void resizeEvent(QResizeEvent*);

private slots:

	void start_server();	// starts the server, at the first time creates the playlist
	void stop_server();		// stop the server

private:
	int				m_server_port;
	std::string		m_scenes_dir;
	std::string		m_scene_suffix;

	QStackedWidget*	m_widget_stacked;
	QWidget*		m_widget_start;
	QWidget*		m_widget_load;
	QWidget*		m_widget_main;

	QAction*		m_action_start_server;
	QAction*		m_action_stop_server;

	QPushButton*	m_button_start_server;
	QLineEdit*		m_lineEdit_server_port;
	QLineEdit*		m_lineEdit_scene_suffix;
	QLineEdit*		m_lineEdit_scenes_dir;


	void 		createWidgetStart();
	void 		createWidgetLoad();
	void 		createWidgetMain();
	void 		createMenu();
};

#endif
