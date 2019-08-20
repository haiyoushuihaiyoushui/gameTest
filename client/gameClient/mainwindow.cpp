#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initTcpConnect();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initTcpConnect()
{
    m_tcpClientCom = new TcpClientCom(SERVER_IP, SERVER_PORT);
    connect(m_tcpClientCom, &TcpClientCom::hasNewDataFromServer, this, &MainWindow::getDataFromServer);
    m_tcpClientCom->conectServer();
}

void MainWindow::on_pushButton_clicked()
{
    m_tcpClientCom->sendDataToServer(QString("HELLO"));
}

void MainWindow::getDataFromServer()
{
    qDebug() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << m_tcpClientCom->getDataFromBuffer();
}
