#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TcpClientCom/tcpclientcom.h"

namespace Ui {
class MainWindow;
}

const QString SERVER_IP = "127.0.0.1";
const qint32 SERVER_PORT = 8080;

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

private:
    Ui::MainWindow *ui;
    TcpClientCom *m_tcpClientCom;
};

#endif // MAINWINDOW_H
