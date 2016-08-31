#include "muskat.h"

using namespace boost::filesystem;

MainWindow::MainWindow() {
	setMinimumSize(1024, 768);
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
	createWidgetMain();
	createMenu();
	
	setCentralWidget(m_widget_stacked);

	m_serverDeamon 	= nullptr;
	m_playlist		= nullptr;
	m_config		= nullptr;
	m_compressor	= nullptr;
	m_filerenderer	= nullptr;
	m_filerenderer	= new FileRenderer;
}

MainWindow::~MainWindow() {
	SAFE_DELETE(m_serverDeamon);
	SAFE_DELETE(m_playlist);
	SAFE_DELETE(m_config);
	SAFE_DELETE(m_compressor);
	SAFE_DELETE(m_filerenderer);
}

void MainWindow::resizeEvent(QResizeEvent*) {}

void MainWindow::createWidgetStart() {
	m_widget_start	  = new QWidget;
	
	QGridLayout* gridLayout = new QGridLayout;
	QGroupBox*	 groupBox	= new QGroupBox;
	QFormLayout* formLayout = new QFormLayout;

	m_lineEdit_server_port  = new QLineEdit("1234");
	m_lineEdit_scenes_dir	= new QLineEdit("../../Scenes");
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

void MainWindow::createWidgetMain() {
	m_widget_main	  		= new QWidget;
	QGridLayout* gridLayout = new QGridLayout;

	gridLayout->addWidget(createWidgetScene(), 		0, 0);
	gridLayout->addWidget(createWidgetTransport(), 0, 1);

	m_widget_main->setLayout(gridLayout);
	m_widget_stacked->addWidget(m_widget_main);
}

QGroupBox* MainWindow::createWidgetScene() {
	QGroupBox* 	groupBox 	= new QGroupBox(tr("scene"));
	QFormLayout* formLayout = new QFormLayout;

	groupBox->setLayout(formLayout);
	return groupBox;
}

QGroupBox* MainWindow::createWidgetTransport() {
	QGroupBox* 		groupBox = new QGroupBox("transport");

	return groupBox;
}

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
	if(m_widget_stacked->currentIndex() == 0) {
		m_server_port  	= m_lineEdit_server_port->text().toInt();
		m_scenes_dir	= m_lineEdit_scenes_dir->text().toStdString();
		m_scene_suffix 	= m_lineEdit_scene_suffix->text().toStdString();

		m_playlist 		= new Playlist(m_scenes_dir, m_scene_suffix);
		m_config		= new Config();
		m_compressor	= new Compressor(m_config);
		m_filerenderer->setScene(&m_playlist->m_scenes[0]);
	}

	m_widget_stacked->setCurrentIndex(1);

	SAFE_DELETE(m_serverDeamon);
	m_serverDeamon = new ServerDeamon(this, m_server_port);
}

void MainWindow::stop_server() {
	SAFE_DELETE(m_serverDeamon);
}


