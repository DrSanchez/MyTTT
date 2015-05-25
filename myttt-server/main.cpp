#include <QCoreApplication>

#include "tttserver.h"
#include <QThread>
#include <QThreadPool>
#include <signal.h>
#include <QDebug>

//allow sighandler to see this object
static TTTServer * server = nullptr;

static void SIG_HANDLER(int signo)
{
    if (signo == SIGINT)
    {
        qDebug() << "Cleaning...";
        server->cleanupServer();
        QThreadPool::globalInstance()->waitForDone(5000);
        QThread::sleep(5);//seconds
        //give server plenty of time to cleanup
        qDebug() << "After waiting, but before deletion...";
        delete server;
        server = nullptr;
        qDebug() << "Done... Can now close hosting terminal...";
    }
    else
        fprintf(stderr, "Received unexpected signal...\n");
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (signal(SIGINT, SIG_HANDLER) == SIG_ERR)
    {
        perror("Cannot catch signal...");
        exit(EXIT_FAILURE);
    }

    //this starts the server
    server = new TTTServer();

    return a.exec();
}
