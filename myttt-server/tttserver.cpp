#include "tttserver.h"
#include <QThreadPool>

//debug include
#include <QDebug>

TTTServer::TTTServer(QObject *parent)
    : QObject(parent), _running(false), _byteBuffer(nullptr),
      _clientList(nullptr), _gameMap(nullptr)
{
    for (int i = 0; i < BUFFER_MAX; i++)
        _basicBuffer[i] = '\0';
    _byteBuffer = new QByteArray();
    _clientList = new QList<ClientObject>();
    _gameMap = new QMap<int, GameManager>();

    _setup = (struct sockaddr_in *)malloc(sizeof(_setup));
    _setup->sin_family = AF_INET;
    _setup->sin_addr.s_addr = htonl(INADDR_ANY);
    _setup->sin_port = htons(42040);


    if ((_listener = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        qDebug() << "Error creating listener...";
    }
    else
    {
        int yes = 1;//magic for following call....
        setsockopt(_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(_listener, (struct sockaddr *)_setup, sizeof(*_setup)) < 0)
        {//this listener didnt work
            close(_listener);
            qDebug() << "Failed to bind listener...";
        }
    }

    //time to start initial listening
    if (listen(_listener, LISTEN_QUEUE) == -1)
    {
        perror("Error in listen...");
        exit(EXIT_FAILURE);
    }

    qDebug() << "Listening...";

    //add listener to master set of descriptors
    FD_SET(_listener, &_master);

    //largest descriptor
    _fdMax = _listener;
    qDebug() << "Listener descriptor: " << _listener;
    //ready to run
    _running = true;

    //server listener prepared, begin server thread
    QThreadPool::globalInstance()->start(this);
}

TTTServer::~TTTServer()
{
}

void TTTServer::run()
{
    int newDescriptor = 0;
    int selectVal = 0;
    int readBytes = 0;
    fd_set readers;

    FD_ZERO(&readers);

    qDebug() << "Server thread running...";

    while(_running)
    {
        //grab master view for select
        readers = _master;

        //we only need to worry about readers
        if ((selectVal = select(_fdMax + 1, &readers, NULL, NULL, NULL)) == -1)
        {
            //are we just kidding?
            if (errno == EINTR)
                continue;
            else
            {//an actual error
                perror("Error on select call...");
                exit(EXIT_FAILURE);
            }
        }

        for (int i = 0; i < _fdMax + 1; i++)
        {
            if (FD_ISSET(i, &readers))
            {
                if (i == _listener)
                {//got new connection

                    qDebug() << "Found a new connection...";

                    if ((newDescriptor = accept(_listener, NULL, NULL)) < 0)
                    {
                        perror("Error on accept call...");
                        exit(EXIT_FAILURE);
                    }

                    qDebug() << "Accepted new connection...";

                    //insert descriptor to master list
                    FD_SET(newDescriptor, &_master);

                    //keep track of highest value descriptor
                    if (newDescriptor > _fdMax)
                        _fdMax = newDescriptor;
                    qDebug() << "New maxFD: " << _fdMax;
                }//end new connection
                else
                {//client sent a message
                    readBytes = recv(i, _basicBuffer, BUFFER_MAX, 0);
                    if (readBytes <= 0)
                    {
                        if (readBytes == 0)
                        {//send out any special close signals
                        }
                        else if (readBytes < 0)
                        {
                            perror("Error on recv call...");
                        }
                        close(i);
                        FD_CLR(i, &_master);
                    }
                    else if (readBytes == BUFFER_MAX)
                    {//need to handle more reading
                        //try recv again?
                    }
                    else
                    {//usual case, got expected amounts of data
                        processMessage(i/*_basicBuffer*/);
                    }
                }
            }
        }
    }
}

void TTTServer::processMessage(int dataSocket)
{
    QJsonDocument doc;
    QJsonObject obj;

    //deep copy the recv message
    _byteBuffer->resize(strlen(_basicBuffer));
    qstrcpy(_byteBuffer->data(), _basicBuffer);
    doc = QJsonDocument::fromBinaryData(*_byteBuffer);
    obj = doc.object();

    CommHeader command = (CommHeader) obj["CommHeader"].toInt();

    switch(command)
    {
        case JOIN:
        {
            addUser(dataSocket, obj);
            break;
        }
        case LIST:
        {
            sendList(dataSocket);
            break;
        }
        case LEAVE:
        {
            removeUser(dataSocket, obj);
            break;
        }
        case INVITE:
        {
            inviteUser(dataSocket, obj);
            break;
        }
        default:
        {
            qDebug() << "Unrecognized command header...";
            break;
        }
    }
}

void TTTServer::updateAllList()
{

}

void TTTServer::addUser(int clientSock, QJsonObject & obj)
{
    ClientObject co;

    co._id = (int) obj["ClientID"].toInt();
    co._gameID = -1;
    co._socketID = clientSock;
    co._username = (QString) obj["Username"].toString();
    co._state = CONNECTED;

    _clientList->append(co);
}

void TTTServer::sendList(int clientSock)
{

}

void TTTServer::removeUser(int clientSock, QJsonObject & obj)
{

}

void TTTServer::inviteUser(int clientSock, QJsonObject & obj)
{

}

