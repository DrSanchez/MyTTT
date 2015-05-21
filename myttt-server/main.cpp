#include <QCoreApplication>

#include "tttserver.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TTTServer * server = new TTTServer();

    return a.exec();
}
