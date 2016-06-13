#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include "muskat.h"

class QOpenGLWidget;
class QStatusBar;
class QLineEdit;
class ServerDeamon;

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow();
    ~MainWindow();
	void resizeEvent(QResizeEvent *);

    int             m_server_port;
    ServerDeamon*   m_serverDeamon;

    void showClientIp(QString Ip);

private slots:
    void set_tools_invisible();
    void toggle_tools();
    void start_server();
    void stop_server();

private:
    void createActions();
    void createStackedWidgets();
    void createTabWidgets();
    void createDockWidgets();

    QStackedWidget* m_stacked;
    QTabWidget*     m_tab;
    QDockWidget*    m_dock;

    QOpenGLWidget*  m_glWidget;
    QWidget*        m_widget_server;
    QWidget*        m_widget_config;
    QWidget*        m_network;

	
    QMenu*          viewMenu;
    QAction*        m_qac_show_tools;
    QAction*        m_qac_start_server;
    QAction*        m_qac_stop_server;

    QPushButton*    m_qpb_start_server;
    QLineEdit*      m_qle_server_port;
    QLineEdit*      m_qle_server_port_config;
    bool m_isNewWidget;
};

#endif
