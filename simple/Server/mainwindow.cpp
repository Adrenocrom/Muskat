#include "muskat.h"

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

	m_renderer = new FileRenderer;
	m_renderer->loadScene("../../Scenes/CoolRandom/run_1/coolrandom_info.txt");
	
	cv::Mat img;
}

MainWindow::~MainWindow() {
}

void MainWindow::resizeEvent(QResizeEvent*) {}

void MainWindow::createWidgetStart() {
	m_widget_start	  = new QWidget;
	
	QGridLayout* gridLayout = new QGridLayout;
	QGroupBox*	 groupBox	= new QGroupBox;
	QFormLayout* formLayout = new QFormLayout;

	groupBox->setMinimumWidth(300);
	groupBox->setLayout(formLayout);
	gridLayout->addWidget(groupBox, 0, 0, 0, 0, Qt::AlignHCenter | Qt::AlignVCenter);
	m_widget_start->setLayout(gridLayout);

	m_widget_stacked->addWidget(m_widget_start);
}

void MainWindow::createWidgetMain() {
	m_widget_main	  = new QWidget;
}

void MainWindow::createMenu() {
	const QIcon icon_start_server = QIcon(":/img/start_icon2.png");
   const QIcon icon_stop_server = QIcon(":/img/stop_icon2.png");
    
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
}

void MainWindow::stop_server() {
}
