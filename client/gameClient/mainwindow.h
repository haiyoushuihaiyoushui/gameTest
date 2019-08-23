#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include <QLabel>
#include <QVector>
#include "TcpClientCom/tcpclientcom.h"

namespace Ui {
class MainWindow;
}

const QString SERVER_IP = "127.0.0.1";
const qint32 SERVER_PORT = 56321;
const qint32 TIMER_INTERVAL = 10;
const qint32 FRAME_SIZE = 7;

typedef enum
{
    FRAME_HEAD,
    FRAME_COMMAND,
    FRAME_DATA,
}FRAME;

typedef enum
{
    KEY_TYPE_UP,
    KEY_TYPE_DOWN,
    KEY_TYPE_LEFT,
    KEY_TYPE_RIGHT,
}KEY_TYPE;

#pragma pack (1)
typedef struct
{
    uint8_t head;
    uint8_t command;
    uint8_t id;
    int16_t x;
    int16_t y;
}FRAME_POSITION_S;
#pragma pack ()

typedef struct
{
    QString id;
    QLabel *label;
}LABEL_INFO_S;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void sendPosition();
    void getDataFromServer();
    void moveLabel();

private:
    void initTcpConnect();

    void createOtherLabel(QString id, int16_t x, int16_t y);
    void distroyOtherLabel(QString id);
    void moveOtherLabel(QString id, int16_t x, int16_t y);

private:
    Ui::MainWindow *ui;
    TcpClientCom *m_tcpClientCom;
    quint8 m_id;
    QTimer m_timer;
    qint32 m_directionFlag;
    QVector<LABEL_INFO_S> m_label;
};

#endif // MAINWINDOW_H
