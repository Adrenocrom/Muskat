/***********************************************************
 *
 *
 *						MAINWINDOW SOURCE
 *					 ======================
 *
 *		AUTHOR: Josef Schulz
 *
 *		Defines the MainWindow class.
 *		Handles the gui and starts the hole server
 *		communication prozess.
 *
 *
 ***********************************************************/

#include "muskat.h"

using namespace boost::filesystem;

// the constructor create the window and set the pointers to nullptr
MainWindow::MainWindow() {
	setMinimumSize(640, 480);
	setMaximumSize(640, 480);
	setWindowTitle(QString("Server"));

	setStyleSheet(QString("QOpenGLWidget{border: 1px solid #000000;}")+
                  QString("QMainWindow{background-color: #ffffff;}")+
                  QString("QMenuBar{background-color: #444444; color: #FFFFFF;}")+
                  QString("QMenuBar::item{background-color: #444444; color: #FFFFFF;}")+
                  QString("QMenuBar::item:selected{background-color: #333333; color: #FFFFFF;}")+
                  QString("QMenu{background-color: #444444; color: #FFFFFF; selection-color: #FFFFFF; selection-background-color: #333333;}")+
                  QString("QMenu::item:disabled{background-color: #444444; color: #AAAAAA;}")+
                  QString("QLabel{color: #222222;}")+
                  QString("QPushButton{color: #FFFFFF; background-color: #222222; border-radius: 5px;}")+
                  QString("QPushButton:pressed{color: #FFFFFF; background-color: #555555;}")+
                  QString("QLineEdit{background-color: #DDDDDD; border-radius: 2px; border: 1px solid #000000;}")+
                  QString("QTextEdit{background-color: #DDDDDD; border-radius: 2px; border: 1px solid #000000;}")+
                  QString("QSlider{background-color: #FFFFFF; color: #FFFFFF; border-style: none;}")+
                  QString("QSlider::handle{background: #222222; border: 1px solid #FFFFFF; width: 5px;}")+
                  QString("QProgressBar{border-radius: 2px; border: 1px solid #000000; color:#000000;text-align: center;}")+
                  QString("QProgressBar::chunk{width: 10px; background-color: #22AA33}")+
                  QString("QComboBox{background-color: #222222; color: #FFFFFF; border-radius: 2px;}") +
                  QString("QComboBox:focus{background-color: #444444; color: #FFFFFF; border-radius: 2px;}"));

	m_widget_stacked = new QStackedWidget;

	createWidgetStart();
	createWidgetLoad();
	createWidgetMain();
	createMenu();
	
	setCentralWidget(m_widget_stacked);

	m_serverDeamon 	= nullptr;
	m_playlist		= nullptr;
	m_config		= nullptr;
	m_compressor	= nullptr;
	m_evaluator		= nullptr;
	m_evaluator		= nullptr;
	m_filerenderer	= nullptr;
	m_filerenderer	= new FileRenderer;
}

// destroys everything
MainWindow::~MainWindow() {
	SAFE_DELETE(m_serverDeamon);
	SAFE_DELETE(m_playlist);
	SAFE_DELETE(m_config);
	SAFE_DELETE(m_compressor);
	SAFE_DELETE(m_filerenderer);
	SAFE_DELETE(m_evaluator);
}

void MainWindow::resizeEvent(QResizeEvent*) {}

// first displayed dialog, gathers information
// about port and scenes. Suffix could be used
// for different type of scenes and renderes.
// Here its just a dummy
void MainWindow::createWidgetStart() {
	m_widget_start	  = new QWidget;
	
	QGridLayout* gridLayout = new QGridLayout;
	QGroupBox*	 groupBox	= new QGroupBox;
	QFormLayout* formLayout = new QFormLayout;

	m_lineEdit_server_port  = new QLineEdit("1234");
	m_lineEdit_scenes_dir	= new QLineEdit("../../../data/Scenes");
	m_lineEdit_scene_suffix = new QLineEdit("_info.txt");
	m_button_start_server	= new QPushButton("start");
	m_button_start_server->setMinimumHeight(25);
	connect(m_button_start_server, &QPushButton::pressed, this, &MainWindow::start_server);

	formLayout->addRow(new QLabel("Port:"), m_lineEdit_server_port);
	formLayout->addRow(new QLabel("Scenes:"), m_lineEdit_scenes_dir);
	formLayout->addRow(new QLabel("Suffix:"), m_lineEdit_scene_suffix);
	formLayout->addRow(new QLabel(""), m_button_start_server);

	groupBox->setMinimumWidth(300);
	groupBox->setLayout(formLayout);
	gridLayout->addWidget(groupBox, 0, 0, 0, 0, Qt::AlignHCenter | Qt::AlignVCenter);
	
	m_widget_start->setLayout(gridLayout);
	m_widget_stacked->addWidget(m_widget_start);
}

// dialog shows a progressbar while the scenes are loaded
void MainWindow::createWidgetLoad() {
	m_widget_load			= new QWidget;
	QGridLayout* gridLayout = new QGridLayout;

	m_progress_load 		= new QProgressBar;
	m_progress_load->setMinimumWidth(300);
	m_progress_load->setRange(0, 100);
	m_progress_load->setValue(0);
	gridLayout->addWidget(m_progress_load, 0, 0, 0, 0, Qt::AlignHCenter | Qt::AlignVCenter);
	
	m_widget_load->setLayout(gridLayout);
	m_widget_stacked->addWidget(m_widget_load);
}

// main dialog, actually nothing happens in here
void MainWindow::createWidgetMain() {
	m_widget_main	  		= new QWidget;
	QGridLayout* gridLayout = new QGridLayout;

	m_widget_main->setLayout(gridLayout);
	m_widget_stacked->addWidget(m_widget_main);
}

// creates the menu of the mainwindow
void MainWindow::createMenu() {
	const QIcon icon_start_server = QIcon(":/img/start_icon.png");
   	const QIcon icon_stop_server = QIcon(":/img/stop_icon.png");
    
	m_action_start_server = new QAction(icon_start_server, tr("&Start"), this);
   	m_action_stop_server = new QAction(icon_stop_server, tr("Stop"), this);
   	m_action_stop_server->setDisabled(true);

	QMenu* menu_file = menuBar()->addMenu(tr("&Server"));

	connect(m_action_start_server, &QAction::triggered, this, &MainWindow::start_server);
   	connect(m_action_stop_server,  &QAction::triggered, this, &MainWindow::stop_server);

   	menu_file->addAction(m_action_start_server);
  	menu_file->addAction(m_action_stop_server);
   	menu_file->addSeparator();

   	QAction* action_quit = menu_file->addAction(tr("&Quit"), this, &QWidget::close);
   	action_quit->setShortcuts(QKeySequence::Quit);
   	action_quit->setStatusTip(tr("Quit the application"));
} 

void MainWindow::start_server() {
	
	// If is started the first time, port and dir will needed from
	// the first dialog to create instance of the playlist.
	if(m_widget_stacked->currentIndex() == 0) {
		m_server_port  	= m_lineEdit_server_port->text().toInt();
		m_scenes_dir	= m_lineEdit_scenes_dir->text().toStdString();
		m_scene_suffix 	= m_lineEdit_scene_suffix->text().toStdString();

		// change to load scenes dialog
		m_widget_stacked->setCurrentIndex(1);
		this->repaint();

		m_playlist 		= new Playlist(this, m_scenes_dir, m_scene_suffix);
		m_config		= new Config(this);
		m_compressor	= new Compressor(m_config);
		m_evaluator		= new Evaluator(m_config, m_compressor);
		m_filerenderer->setScene(&m_playlist->m_scenes[0]);
		
		// after creating everything change to the main dialog.
		m_widget_stacked->setCurrentIndex(2);
	}


	// create or recreate the deamon
	SAFE_DELETE(m_serverDeamon);
	m_serverDeamon = new ServerDeamon(this, m_server_port);

	// set dialog options
   	m_action_stop_server->setDisabled(false);
   	m_action_start_server->setDisabled(true);
}

void MainWindow::stop_server() {
	// delete deamon
	SAFE_DELETE(m_serverDeamon);
	
	// set dialog options
   	m_action_stop_server->setDisabled(true);
   	m_action_start_server->setDisabled(false);
}


