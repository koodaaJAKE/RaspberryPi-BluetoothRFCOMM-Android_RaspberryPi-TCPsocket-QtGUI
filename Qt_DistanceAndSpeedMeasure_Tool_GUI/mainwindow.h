#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TCP_SocketClient.h"

namespace Ui {
class MainWindow;
}

extern int g_CheckConnectionStatus;

class MainWindow : public QMainWindow, public TCP_SocketClient
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    int on_DistanceButton_clicked();

    int on_SpeedButton_clicked();

    int on_QuitButton_clicked();

    int on_ConnectButton_clicked();

    int on_DistanceSpeedButton_clicked();

    int on_ClearScreenButton_clicked();

private:
    Ui::MainWindow *ui;
    bool m_checkConnectionStatus;

    HRLVEZ0_Data_t m_Data;
};

#endif // MAINWINDOW_H
