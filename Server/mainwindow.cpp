#include "muskat.h"

MainWindow::MainWindow() : m_stacked(new QStackedWidget) {
    setMinimumSize(1024, 768);
    setWindowTitle(QString("Server"));
    setStyleSheet(QString("QOpenGLWidget{border: 1px solid #000000;}")+
                  QString("QMainWindow{background-color: #ffffff; border: 1px solid #000000;}")+
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

    m_server_port = 1234;
    setCentralWidget(m_stacked);

    createStackedWidgets();
    createActions();
    createTabWidgets();
    createDockWidgets();

    statusBar()->showMessage(tr("ready"));

    m_serverDeamon = NULL;
}

void MainWindow::createActions() {
    QMenu* fileMenu = menuBar()->addMenu(tr("&Server"));
    const QIcon start_server_icon = QIcon::fromTheme("document-open", QIcon(":/images/start_icon2.png"));
    m_qac_start_server = new QAction(start_server_icon, tr("&Start"), this);
    const QIcon stop_server_icon = QIcon::fromTheme("document-open", QIcon(":/images/stop_icon2.png"));
    m_qac_stop_server = new QAction(stop_server_icon, tr("Stop"), this);
    m_qac_stop_server->setDisabled(true);

    connect(m_qac_start_server, &QAction::triggered, this, &MainWindow::start_server);
    connect(m_qac_stop_server, &QAction::triggered, this, &MainWindow::stop_server);
    fileMenu->addAction(m_qac_start_server);
    fileMenu->addAction(m_qac_stop_server);
    fileMenu->addSeparator();
    QAction* quitAct = fileMenu->addAction(tr("&Quit"), this, &QWidget::close);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit the application"));

    viewMenu = menuBar()->addMenu(tr("&View"));
    m_qac_show_tools = new QAction(tr("&show Tools"), this);
    m_qac_show_tools->setCheckable(true);
    m_qac_show_tools->setDisabled(true);
    connect(m_qac_show_tools, &QAction::triggered, this, &MainWindow::toggle_tools);
    viewMenu->addAction(m_qac_show_tools);
}

void MainWindow::createStackedWidgets() {
    m_widget_server = new QWidget(this);
    QGridLayout* gLayout = new QGridLayout(this);
    QGroupBox* gb1 = new QGroupBox();
    gb1->setMinimumWidth(300);
    QFormLayout* fLayout = new QFormLayout;
    m_qle_server_port = new QLineEdit("");
    m_qle_server_port->setText(QString::number(m_server_port));
    QLineEdit* qle_scene = new QLineEdit("default");
    m_qpb_start_server = new QPushButton("start");
    m_qpb_start_server->setMinimumHeight(25);
    connect(m_qpb_start_server, &QPushButton::pressed, this, &MainWindow::start_server);

    fLayout->addRow(new QLabel(tr("Port:")), m_qle_server_port);
    fLayout->addRow(new QLabel(tr("Scene:")), qle_scene);
    fLayout->addRow(new QLabel(tr("")), m_qpb_start_server);
    //fLayout->addRow(m_qpb_start_server);
    gb1->setLayout(fLayout);
    gLayout->addWidget(gb1,0 ,0 ,0 ,0 , Qt::AlignHCenter | Qt::AlignVCenter);
    m_widget_server->setLayout(gLayout);  

    m_glWidget = new GLWidget(this);
    m_glWidget->setMinimumSize(640, 480);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    scrollArea->setWidget(m_glWidget);

    m_stacked->addWidget(m_widget_server);
    m_stacked->addWidget(scrollArea);
}

void MainWindow::createTabWidgets() {
    m_widget_config = new QWidget(this);
    QFormLayout* fl1 = new QFormLayout;
    m_qle_server_port_config = new QLineEdit(QString::number(m_server_port));
    m_qle_widget_width_config = new QLineEdit(QString::number(m_glWidget->width()));
    m_qle_widget_height_config = new QLineEdit(QString::number(m_glWidget->height()));
    m_qpb_readpixel = new QPushButton("get pixel");
    m_qpb_readpixel->setMinimumHeight(25);
    connect(m_qpb_readpixel, &QPushButton::pressed, this, &MainWindow::get_pixel);

    m_qte_jpegBase64 = new QTextEdit();
    m_qle_jpegImageQuality = new QLineEdit();
    m_qle_jpegImageQuality->setText("100");
    m_qle_jpegImgageSize = new QLineEdit();
    m_qle_num_clients = new QLineEdit();
    m_qle_num_clients->setText("0");

    fl1->addRow(new QLabel("Port:"), m_qle_server_port_config);
    fl1->addRow(new QLabel("Clients:"), m_qle_num_clients);
    fl1->addRow(new QLabel("Width:"), m_qle_widget_width_config);
    fl1->addRow(new QLabel("Height:"), m_qle_widget_height_config);
    fl1->addRow(m_qpb_readpixel);
    fl1->addRow(new QLabel("Quality"), m_qle_jpegImageQuality);
    fl1->addRow(new QLabel("Size"), m_qle_jpegImgageSize);
    fl1->addRow(m_qte_jpegBase64);
    m_widget_config->setLayout(fl1);

    m_tab = new QTabWidget(this);
    m_tab->setMinimumWidth(300);
    m_tab->addTab(m_widget_config, tr("configuration"));
}

void MainWindow::createDockWidgets() {
    m_dock = new QDockWidget(tr("Tools"), this);
    m_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_dock->setWidget(m_tab);
    addDockWidget(Qt::LeftDockWidgetArea, m_dock);
    m_dock->setVisible(false);
    connect(m_dock, &QDockWidget::visibilityChanged, this, &MainWindow::set_tools_invisible);
}

MainWindow::~MainWindow() {

}

void MainWindow::start_server() {
    statusBar()->showMessage(tr("starting server ..."));
    m_stacked->setCurrentIndex(1);
    m_dock->setVisible(true);
    m_qac_show_tools->setDisabled(false);
    m_qac_show_tools->setChecked(true);

    if(m_serverDeamon) {
        delete m_serverDeamon;
        m_serverDeamon = NULL;
    }

    m_serverDeamon = new ServerDeamon(this, m_server_port);

    if(m_serverDeamon) {
        m_qac_start_server->setDisabled(true);
        m_qac_stop_server->setDisabled(false);
        statusBar()->showMessage("server listen on port " + QString::number(m_server_port));
    }
    m_glWidget->update();
}

void MainWindow::stop_server() {
    statusBar()->showMessage("stopping server ...");
    if(m_serverDeamon) {
        delete m_serverDeamon;
        m_serverDeamon = NULL;
    }

    m_qac_start_server->setDisabled(false);
    m_qac_stop_server->setDisabled(true);
    statusBar()->showMessage("server stoped");
}

void MainWindow::toggle_tools() {
    if(m_qac_show_tools->isChecked() == false) {
        m_dock->setVisible(false);
        m_qac_show_tools->setChecked(false);
    }
    else {
        m_dock->setVisible(true);
        m_qac_show_tools->setChecked(true);
    }
}

void MainWindow::set_tools_invisible() {
    if(m_dock->isVisible()) m_qac_show_tools->setChecked(true);
    else m_qac_show_tools->setChecked(false);
}

void MainWindow::resizeEvent(QResizeEvent *) {

}

void MainWindow::get_pixel() {
    SBuffer buffer;
    buffer.rgb = new GLubyte[m_glWidget->width() * m_glWidget->height() * 3];
    m_glWidget->renderFrame(&buffer);

    long unsigned int _jpegSize = 0;
    unsigned char* _compressedImage = NULL;

    tjhandle _jpegCompressor = tjInitCompress();

    tjCompress2(_jpegCompressor,
                buffer.rgb,
                m_glWidget->width(),
                0,
                m_glWidget->height(),
                TJPF_RGB,
                &_compressedImage,
                &_jpegSize,
                TJSAMP_444,
                m_qle_jpegImageQuality->text().toInt(),
                TJFLAG_FASTDCT);

    tjDestroy(_jpegCompressor);

    QByteArray test;
    test.append((const char*)_compressedImage, _jpegSize);

    m_qle_jpegImgageSize->setText(QString::number(_jpegSize) + "Bytes");
    m_qte_jpegBase64->setText(QString(test.toBase64()));

    tjFree(_compressedImage);

    if(buffer.rgb) {
        delete[] buffer.rgb;
        buffer.rgb = NULL;
    }
}
