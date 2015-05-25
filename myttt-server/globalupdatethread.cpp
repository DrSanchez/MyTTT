#include "globalupdatethread.h"
#include <sys/socket.h>
#include <QDebug>

GlobalUpdateThread::GlobalUpdateThread(QObject *parent)
    : QObject(parent), _bytes(nullptr), _socketsToSend(nullptr)
{
    _bytes = new QByteArray();
    _socketsToSend = new QList<int>();
}

void GlobalUpdateThread::addSocket(int sock)
{
    _socketsToSend->append(sock);
}

void GlobalUpdateThread::setBytes(char * data)
{
    _bytes->resize(qstrlen(data) + 1);
    qstrcpy(_bytes->data(), data);
}

void GlobalUpdateThread::clearSocketSet()
{
    _socketsToSend->clear();
}

void GlobalUpdateThread::run()
{
    bool error = false;
    int bytesWritten = 0;
    int bytesReturned = 0;
    int messageFullLength = _bytes->length();
    for (int sock : *_socketsToSend)
    {
        qDebug() << "Sending globally to socket: " << sock;
        QByteArray temp;
        temp.resize(messageFullLength + 1);
        qstrcpy(temp.data(), _bytes->data());

        while (bytesWritten < messageFullLength && !error)
        {
            qDebug() << "Writing: " << temp;
            bytesReturned = send(sock, temp.data(), temp.length(), 0);
            if (bytesReturned < 0)
            {
                error = true;//for this socket
                //need to figure out good handling for this
            }
            bytesWritten += bytesReturned;
            if (bytesWritten < messageFullLength)
                temp.remove(0, bytesReturned - 1);
            bytesReturned = 0;
        }
        bytesWritten = 0;
        bytesReturned = 0;
        error = false;
    }
}
