#include <QCoreApplication>

#include "tttserver.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //annoying "unused" warning.. this starts the server
    TTTServer * server = new TTTServer();

    return a.exec();
}
