#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include <QMainWindow>

class QStackedWidget;
class AbstractRenderer;
class FileRenderer;

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	AbstractRenderer* m_renderer;
	FileRenderer*		m_fileRenderer;

	MainWindow();
	~MainWindow();
	
	void resizeEvent(QResizeEvent*);

private slots:

	void start_server();
	void stop_server();

private:
	
	QStackedWidget*	m_widget_stacked;
	QWidget*				m_widget_start;
	QWidget*				m_widget_main;

	QAction*				m_action_start_server;
	QAction*				m_action_stop_server;

	void createWidgetStart();
	void createWidgetMain();
	void createMenu();
};

#endif
