#include "tttserver.h"
#include <QThreadPool>

//debug include
#include <QDebug>

TTTServer::TTTServer(QObject *parent)
    : QObject(parent), _uniquifier(0), _running(false),
      _byteBuffer(nullptr), _clientList(nullptr), _gameMap(nullptr),
      _global(nullptr)
{
    for (int i = 0; i < BUFFER_MAX; i++)
        _basicBuffer[i] = '\0';
    _byteBuffer = new QByteArray();
    _clientList = new QList<ClientObject *>();
    _gameMap = new QMap<int, GameManager>();
    _global = new GlobalUpdateThread(this);
    _global->setAutoDelete(false);
    //tell the thread pool to give us a few threads on deck
    QThreadPool::globalInstance()->setMaxThreadCount(6);

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

    connect(this, &TTTServer::sendGlobalUpdate, this, &TTTServer::issueGlobalUpdate);

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
    ssize_t readBytes = 0;
    fd_set readers;
    char buffer[BUFFER_MAX] = { '\0' };

    FD_ZERO(&readers);

    qDebug() << "Server thread running...";

    while(_running)
    {
        //grab master view for select
        FD_ZERO(&readers);
        readers = _master;
        clearBuffer(buffer);
        readBytes = 0;

        //we only need to worry about readers
        if ((selectVal = select(_fdMax + 1, &readers, NULL, NULL, NULL)) == -1)
        {
            //are we just kidding?
            if (errno == EINTR)
                continue;
            else
            {//an actual error
                qDebug() << "Error on select call...";
                exit(EXIT_FAILURE);
            }
        }

        if (FD_ISSET(_listener, &readers))
        {//got new connection
            if ((newDescriptor = accept(_listener, NULL, NULL)) < 0)
            {
                perror("Error on accept call...");
                exit(EXIT_FAILURE);
            }

            qDebug() << "Accepted new connection..." << newDescriptor;

            ClientObject * newClient = new ClientObject();
            newClient->_socketID = newDescriptor;
            _clientList->append(newClient);

            //insert descriptor to master list
            FD_SET(newDescriptor, &_master);

            //keep track of highest value descriptor
            if (newDescriptor > _fdMax)
                _fdMax = newDescriptor;
        }//end new connection

        //must dereference list object for c++11 loop
        for (ClientObject * looper : *_clientList)
        {
            if (FD_ISSET(looper->_socketID, &readers))
            {//client sent a message
                readBytes = recv(looper->_socketID, buffer, BUFFER_MAX, 0);
                qDebug() << "Another Received: " << buffer << readBytes;
                if (readBytes <= 0)
                {
                    if (readBytes == 0)
                    {//send out any special close signals
                    }
                    else if (readBytes < 0)
                    {
                        perror("Error on recv call...");
                    }
                    close(looper->_socketID);
                    FD_CLR(looper->_socketID, &_master);
                }
                else if (readBytes >= BUFFER_MAX)
                {//need to handle more reading
                    char * bufferCollector;

                    //deep copy the recv message
                    _byteBuffer->resize(qstrlen(buffer));
                    qstrcpy(_byteBuffer->data(), buffer);

                    //won't ever actually be greater than max, but QByteArray could handle the case
                    while (readBytes >= BUFFER_MAX)
                    {
                        readBytes = recv(looper->_socketID, buffer, BUFFER_MAX, 0);

                        //ensure any fragmented messages are all deep copied into our buffers
                        bufferCollector = new char[_byteBuffer->size() + qstrlen(buffer) + 1];
                        qstrcpy(bufferCollector, _byteBuffer->data());
                        strcat(bufferCollector, buffer);
                        _byteBuffer->resize(qstrlen(bufferCollector));
                        qstrcpy(_byteBuffer->data(), bufferCollector);
                    }
                    processMessage(looper);
                }
                else //readBytes < BUFFER_MAX
                {//usual case, got expected amounts of data
                    //deep copy the recv message
                    _byteBuffer->resize(qstrlen(buffer));
                    qstrcpy(_byteBuffer->data(), buffer);
                    processMessage(looper);
                }
            }
        }
    }
}

void TTTServer::processMessage(ClientObject * client)
{
    QJsonDocument doc;
    QJsonObject obj;

    //every case puts the data in the bytebuffer before calling this method
    doc = QJsonDocument::fromJson(*_byteBuffer);
    obj = doc.object();

    CommHeader command = (CommHeader) obj["CommHeader"].toInt();

    switch(command)
    {
        case JOIN:
        {
            addUser(client->_socketID, obj);
            break;
        }
        case LIST:
        {
            sendList(client->_socketID);
            break;
        }
        case LEAVE:
        {
            removeUser(client->_socketID, obj);
            break;
        }
        case INVITE:
        {
            inviteUser(client->_socketID, obj);
            break;
        }
        case ACCEPTINVITE:
        {
            startGame(client->_socketID, obj);
        }
        default:
        {
            qDebug() << "Unrecognized command header...";
            break;
        }
    }
}

void TTTServer::issueGlobalUpdate()
{
    _global->setBytes(_byteBuffer->data());
    _global->clearSocketSet();
    for (ClientObject * obj : *_clientList)
        if (obj->_socketID != _listener)
            _global->addSocket(obj->_socketID);
    QThreadPool::globalInstance()->start(_global);
}

void TTTServer::addUser(int clientSock, QJsonObject & obj)
{
    ClientObject * co;
    if ((co = findClientBySocket(clientSock)) == nullptr)
    {//error, socket not in list, should not occur

    }

    //assign client values
    co->_gameID = -1;
    co->_socketID = clientSock;
    co->_username = obj["Username"].toString();
    co->_state = CONNECTED;

    //check username
    for (ClientObject * check : *_clientList)
    {
        //dont check yourself
        if (co->_socketID != check->_socketID)
            if (co->_username == check->_username)//simple handle username conflict
                co->_username = (co->_username + QString::number(_uniquifier++));
    }

    qDebug() << co->_username << " has joined the server...\n"
             << "Sending new user to clients...";

    //create new json data for client list
    QJsonObject o;
    QJsonDocument d;
    o["ServerResponse"] = ACCEPTEDCLIENT;
    o["Username"] = co->_username;

    d.setObject(o);
    *_byteBuffer = d.toJson();
    emit sendGlobalUpdate();
}

void TTTServer::sendList(int clientSock)
{
    QJsonObject o;
    QJsonDocument d;
    QStringList userlist;
    QStringList userStatus;

    o["ServerResponse"] = USERLIST;

    for (ClientObject * obj : *_clientList)
    {
        userStatus.append((obj->_gameID != -1) ? "true" : "false");
        userlist.append(obj->_username);
    }

    QVariant var(userlist);
    o["Userlist"] = QJsonValue::fromVariant(var);

    QVariant var1(userStatus);
    o["Statuslist"] = QJsonValue::fromVariant(var1);

    qDebug() << "Client requested list of users...";

    d.setObject(o);
    *_byteBuffer = d.toJson();
    if (!sendAll(clientSock))
        qDebug() << "Error sending user list...";
}

void TTTServer::removeUser(int clientSock, QJsonObject & obj)
{

}

void TTTServer::inviteUser(int clientSock, QJsonObject & obj)
{

}

void TTTServer::startGame(int clientSock, QJsonObject & obj)
{

}

ClientObject * TTTServer::findClientBySocket(int clientSock)
{
    ClientObject * found = nullptr;

    //check for this as a problem spot, can we return a pointer gather from ranged for loop?
    for (ClientObject * obj : *_clientList)
    {
        if (obj->_socketID == clientSock)
            found = obj;
    }

    return found;
}

bool TTTServer::sendAll(int receiver)
{
    bool success = true;
    int bytesWritten = 0;
    int bytesReturned = 0;
    int messageFullLength = _byteBuffer->length();
    do
    {
        bytesReturned = send(receiver, _byteBuffer->data(), _byteBuffer->length(), 0);

        if (bytesReturned < 0)
        {
            success = false;
            break;
        }
        bytesWritten += bytesReturned;
        if (bytesWritten < messageFullLength)
            _byteBuffer->remove(0, bytesReturned - 1);
        bytesReturned = 0;
    } while (bytesWritten < messageFullLength && success);
    _byteBuffer->clear();
    return success;
}

void TTTServer::clearBuffer(char * buffer)
{
    for (int i = 0; i < BUFFER_MAX; i++)
        buffer[i] = '\0';
}
