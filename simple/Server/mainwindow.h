#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include <QMainWindow>

class QStackedWidget;
class AbstractRenderer;
class QGroupBox;
class QPushButton;
class QLineEdit;
class QComboBox;
class QLabel;

namespace boost {
	namespace filesystem {
		class path;
	};
};

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	AbstractRenderer* 		 m_renderer;
	std::vector<std::string> m_scenes;

	MainWindow();
	~MainWindow();
	
	void resizeEvent(QResizeEvent*);

private slots:

	void start_server();
	void stop_server();
	void load_scene();

private:
	int					m_server_port;
	std::string			m_scenes_dir;
	std::string			m_scene_suffix;

	QStackedWidget*	m_widget_stacked;
	QWidget*				m_widget_start;
	QWidget*				m_widget_main;

	QAction*				m_action_start_server;
	QAction*				m_action_stop_server;

	QPushButton*		m_button_start_server;
	QLineEdit*			m_lineEdit_server_port;
	QLineEdit*			m_lineEdit_scene_suffix;
	QLineEdit*			m_lineEdit_scenes_dir;

	QComboBox*			m_comboBox_scenes;
	QPushButton*		m_button_load_scene;
	QLabel*				m_label_num_frames;

	void createWidgetStart();
	void createWidgetMain();
	QGroupBox* createWidgetTransport();
	QGroupBox* createWidgetScene();
	void createMenu();

	std::vector<boost::filesystem::path> getFiles(boost::filesystem::path& p);

	void loadScenesFromDir(std::string d = "../../Scenes", std::string s = "_info.txt");
	void addScenes(boost::filesystem::path& p, std::string s = "_info.txt");
};

#endif
