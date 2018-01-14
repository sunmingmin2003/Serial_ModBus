#include "mainwindow.h"
#include <QApplication>
#include "spreadsheet.h"
#include "cmodbus.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    CModBus  myModBus(PROTO_ASCII);
    w.show();

    return a.exec();
}
