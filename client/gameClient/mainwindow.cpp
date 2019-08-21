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

    getId();
}

void MainWindow::getId()
{
    QByteArray sendData;

    sendData.append(qint8(0xAB));
    sendData.append(qint8(0x01));
    m_tcpClientCom->sendDataToServer(sendData);
}

void MainWindow::on_pushButton_clicked()
{
    QByteArray sendData;
    QString realData = QString(m_id) + QString("+") + QString("(12, 32)");

    sendData.append(qint8(0xAB));
    sendData.append(qint8(0x02));
    sendData.append(realData);
    m_tcpClientCom->sendDataToServer(sendData);
}

void MainWindow::getDataFromServer()
{
    QByteArray recvData;
    recvData = m_tcpClientCom->getDataFromBuffer();
    qDebug() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << recvData;
    if (0xAB == quint8(recvData.at(FRAME_HEAD)))
    {
        switch (quint8(recvData.at(FRAME_COMMAND)))
        {
        case 0x01:
            m_id = recvData.at(FRAME_DATA);
            break;

        case 0x02:
            break;

        default:
            break;
        }
    }
}
