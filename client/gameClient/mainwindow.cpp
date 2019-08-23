#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_directionFlag = 0;
    m_id = '\0';
    setFixedSize(500, 400); // 禁止改变窗口大小
    grabKeyboard();
    initTcpConnect();
    connect(&m_timer, &QTimer::timeout, this, &MainWindow::moveLabel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (Qt::Key_Up != event->key() && Qt::Key_Down != event->key()
            && Qt::Key_Left != event->key() && Qt::Key_Right != event->key())
    {
        return;
    }
    switch (event->key())
    {
    case Qt::Key_Up:
        m_directionFlag = KEY_TYPE_UP;
        break;
    case Qt::Key_Down:
        m_directionFlag = KEY_TYPE_DOWN;
        break;
    case Qt::Key_Left:
        m_directionFlag = KEY_TYPE_LEFT;
        break;
    case Qt::Key_Right:
        m_directionFlag = KEY_TYPE_RIGHT;
        break;
    default:
        break;
    }

    m_timer.start(TIMER_INTERVAL);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (Qt::Key_Up != event->key() && Qt::Key_Down != event->key()
            && Qt::Key_Left != event->key() && Qt::Key_Right != event->key())
    {
        return;
    }
    m_timer.stop();
}

void MainWindow::sendPosition()
{
    QByteArray sendData;
    FRAME_POSITION_S sendFrame;

    sendFrame.head = 0xAB;
    sendFrame.command = 0x03;
    sendFrame.id = m_id;
    sendFrame.x = qint16(ui->label->x());
    sendFrame.y = qint16(ui->label->y());

    sendData.resize(sizeof(FRAME_POSITION_S));
    memcpy(sendData.data(), &sendFrame, sizeof(FRAME_POSITION_S));
    m_tcpClientCom->sendDataToServer(sendData);
}

void MainWindow::initTcpConnect()
{
    m_tcpClientCom = new TcpClientCom(SERVER_IP, SERVER_PORT);
    connect(m_tcpClientCom, &TcpClientCom::hasNewDataFromServer, this, &MainWindow::getDataFromServer);
    m_tcpClientCom->conectServer();
}

void MainWindow::createOtherLabel(QString id, int16_t x, int16_t y)
{
    LABEL_INFO_S newLabelData;
    QLabel *newLabel;

    newLabel = new QLabel(this);
    newLabel->setText(id);
    newLabel->setGeometry(x, y, ui->label->width(), ui->label->height());
    newLabel->show();

    newLabelData.id = id;
    newLabelData.label = newLabel;
    m_label.push_back(newLabelData);
}

void MainWindow::distroyOtherLabel(QString id)
{
    QVector<LABEL_INFO_S>::iterator it;
    for (it=m_label.begin(); it!=m_label.end(); it++)
    {
        if (id == it->id)
        {
            it->label->hide();
            delete it->label;
            m_label.erase(it);
            break;
        }
    }
}

void MainWindow::moveOtherLabel(QString id, int16_t x, int16_t y)
{
    QVector<LABEL_INFO_S>::iterator it;

    for (it=m_label.begin(); it!=m_label.end(); it++)
    {
        if (id == it->id)
        {
            it->label->setGeometry(x, y, ui->label->width(), ui->label->height());
        }
    }
}

void MainWindow::getDataFromServer()
{
    QByteArray recvData;
    QByteArray tmpData;
    FRAME_POSITION_S recvFrame;

    recvData = m_tcpClientCom->getDataFromBuffer();
    for (qint32 i=0; i<recvData.size()/FRAME_SIZE; i++)
    {
        tmpData = recvData.mid(i*FRAME_SIZE, FRAME_SIZE);
        qDebug() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << tmpData << tmpData.size();
        memcpy(&recvFrame, tmpData.data(), sizeof(FRAME_POSITION_S));
        if (0xAB == quint8(tmpData.at(FRAME_HEAD)))
        {
            switch (quint8(tmpData.at(FRAME_COMMAND)))
            {
            case 0x01:
                //判断是否接收到其他新连接
                if ('\0' == m_id)
                {
                    m_id = recvFrame.id;
                    ui->label->setText(QString(m_id));
                    ui->label->setGeometry(recvFrame.x, recvFrame.y, ui->label->height(), ui->label->width());
                }
                else
                {
                    createOtherLabel(QString(recvFrame.id), recvFrame.x, recvFrame.y);
                }
                break;

            case 0x02:
                distroyOtherLabel(QString(recvFrame.id));
                break;

            case 0x03:
                moveOtherLabel(QString(recvFrame.id), recvFrame.x, recvFrame.y);
                break;

            default:
                break;
            }
        }
    }
}

void MainWindow::moveLabel()
{
    qint32 x = ui->label->x();
    qint32 y = ui->label->y();
    switch (m_directionFlag)
    {
    case KEY_TYPE_UP:
        if (y > 0)
        {
            y--;
        }
        break;
    case KEY_TYPE_DOWN:
        if (y < height() - ui->label->height())
        {
            y++;
        }
        break;
    case KEY_TYPE_LEFT:
        if (x > 0)
        {
            x--;
        }
        break;
    case KEY_TYPE_RIGHT:
        if (x < width() - ui->label->width())
        {
            x++;
        }
        break;
    default:
        break;
    }

    ui->label->setGeometry(x, y, ui->label->width(), ui->label->height());
    sendPosition();
}
