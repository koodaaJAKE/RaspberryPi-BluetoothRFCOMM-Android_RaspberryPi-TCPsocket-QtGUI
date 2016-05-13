#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TCP_SocketClient.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_checkConnectionStatus ( false )
{
    ui->setupUi(this);
    connect(ui->DistanceButton, SIGNAL(clicked()), ui->lcdDistance, SLOT(setDecMode()));
    connect(ui->SpeedButton, SIGNAL(clicked()), ui->lcdSpeed, SLOT(setDecMode()));
    connect(ui->DistanceSpeedButton, SIGNAL(clicked()), ui->lcdDistance_2, SLOT(setDecMode()));
    connect(ui->DistanceSpeedButton, SIGNAL(clicked()), ui->lcdSpeed_2, SLOT(setDecMode()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::on_DistanceSpeedButton_clicked()
{
    if(TCP_SocketClient::SendAndReceiveSocketData(&m_Data) == false)
    {
        std::cout << "Couldn't receive the distance data!\n" << std::endl;
        return 1;
    }

    ui->lcdDistance_2->display(m_Data.distance);
    ui->lcdSpeed_2->display(QString::number(m_Data.speed, 'f', 2));
    return 0;
}

int MainWindow::on_DistanceButton_clicked()
{
    if(TCP_SocketClient::SendAndReceiveSocketData(&m_Data) == false)
    {
        std::cout << "Couldn't receive the distance data!\n" << std::endl;
        return 1;
    }

    ui->lcdDistance->display(m_Data.distance);
    return 0;
}

int MainWindow::on_SpeedButton_clicked()
{
    if(TCP_SocketClient::SendAndReceiveSocketData(&m_Data) == false)
    {
        std::cout << "Couldn't receive the speed data!\n" << std::endl;
        return 1;
    }

    /* Set to 2 decimal precision and print it to the lcd*/
    ui->lcdSpeed->display(QString::number(m_Data.speed, 'f', 2));
    return 0;
}

int MainWindow::on_QuitButton_clicked()
{
    if(TCP_SocketClient::SendQuitCommand() == false)
    {
        std::cout << "Couldn't sent the quit command!\n" << std::endl;
        return 1;
    }

    QApplication::quit();

    return 0;
}

int MainWindow::on_ConnectButton_clicked()
{
    /* Check if already connected */
    if(!m_checkConnectionStatus)
    {
        if(TCP_SocketClient::CreateSocket() == false)
        {
            std::cout << "Couldn't connect to the socket!\n" << std::endl;
            return 1;
        }

        if(TCP_SocketClient::SendConnectCommand() == false)
        {
            std::cout << "Couldn't send the connect command!\n" << std::endl;
            return 1;
        }

        sleep(2);

        if(TCP_SocketClient::SendClearLCD_Command() == false)
        {
            std::cout << "Couldn't clear the LCD screen!\n" << std::endl;
            return 1;
        }

    }
    else
    {
        std::cout << "Already connected!\n" << std::endl;

        if(TCP_SocketClient::SendAlreadyConnectedCommand() == false)
        {
            std::cout << "Couldn't send already connected command!\n" << std::endl;
            return 1;
        }

        sleep(1);

        if(TCP_SocketClient::SendClearLCD_Command() == false)
        {
            std::cout << "Couldn't clear the LCD screen!\n" << std::endl;
            return 1;
        }

        return 0;
    }

    m_checkConnectionStatus = true;

    return 0;
}


int MainWindow::on_ClearScreenButton_clicked()
{
    ui->lcdDistance->display(0);
    ui->lcdDistance_2->display(0);
    ui->lcdSpeed->display(0);
    ui->lcdSpeed_2->display(0);

    if(TCP_SocketClient::SendClearLCD_Command() == false)
    {
        std::cout << "Couldn't clear the LCD screen!\n" << std::endl;
        return 1;
    }

    return 0;
}
