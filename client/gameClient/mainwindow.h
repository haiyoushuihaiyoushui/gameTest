#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TcpClientCom/tcpclientcom.h"

namespace Ui {
class MainWindow;
}

const QString SERVER_IP = "127.0.0.1";
const qint32 SERVER_PORT = 56321;

typedef enum
{
    FRAME_HEAD,
    FRAME_COMMAND,
    FRAME_DATA,
}FRAME;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_pushButton_clicked();
    void getDataFromServer();

private:
    void initTcpConnect();
    void getId();

private:
    Ui::MainWindow *ui;
    TcpClientCom *m_tcpClientCom;
    QString m_id;
};

#endif // MAINWINDOW_H
