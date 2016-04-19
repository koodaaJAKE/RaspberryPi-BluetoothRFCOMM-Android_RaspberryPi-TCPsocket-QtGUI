#include "mainwindow.h"
#include <QApplication>
#include "TCP_SocketClient.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    w.show();

    return a.exec();
}
