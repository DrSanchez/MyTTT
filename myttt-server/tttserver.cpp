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
    _gameMap = new QMap<int, GameManager *>();
    _global = new GlobalUpdateThread(this);
    _global->setAutoDelete(false);
    //tell the thread pool to give us a few threads on deck
    QThreadPool::globalInstance()->setMaxThreadCount(6);

    _setup = (struct sockaddr_in *)malloc(sizeof(_setup));
    _setup->sin_family = AF_INET;
    _setup->sin_addr.s_addr = htonl(INADDR_ANY);
    _setup->sin_port = htons(42040);

    //create tcp listener
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

void TTTServer::cleanupServer()
{
    ClientObject * cleaner = nullptr;

    //stop server thread
    _running = false;
    //_global thread either isnt running, or will be done very shortly
    QThreadPool::globalInstance()->waitForDone(5000);
    //can safely force closures now

    //no reason to be listening if we are dying
    shutdown(_listener, 2);
    close(_listener);

    for (ClientObject * inner : *_clientList)
    {
        cleaner = inner;

        shutdown(cleaner->_socketID, 2);//close in/out pipes
        close(cleaner->_socketID);//we can now assume nothing will hold up close
        //client should have received a 0-byte message to close and cleanup
        delete cleaner;
        cleaner = nullptr;
        //go around for next client in list
    }


    //need to double check this, changed to pointers
//    _gameMap->clear();
//    delete _gameMap;


    _gameMap = nullptr;
    _clientList->clear();
    delete _clientList;
    _clientList = nullptr;
    _byteBuffer->clear();
    delete _byteBuffer;
    _byteBuffer = nullptr;

    //since we set autodelete = false,
    delete _global;//we are responsible for deletion of runnable
    _global = nullptr;

    delete _setup;
    _setup = nullptr;

    FD_ZERO(&_master);
    qDebug() << "Cleaned up successfully...";
}

TTTServer::~TTTServer()
{
    //can safely do nothing - protected by cleanup method
    qDebug() << "Destructing nicely...";
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
            removeUser(client->_socketID);
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
            break;
        }
        case DECLINEINVITE:
        {
            declineInvite(client->_socketID, obj);
            break;
        }
        case MAKEMOVE:
        {
            makeMove(client->_socketID, obj);
            break;
        }
        case FORFEIT:
        {
            alertForfeit(client->_socketID, obj);
            break;
        }
        default:
        {
            qDebug() << "Unrecognized command header...";
            break;
        }
    }
}

void TTTServer::alertForfeit(int clientSock, QJsonObject & obj)
{//client socket is caller
    QJsonObject o;
    QJsonDocument d;
    ClientObject * co = findClientBySocket(clientSock);
    GameManager * gm = _gameMap->operator [](co->_gameID);
    int opponent = gm->getOpponentSocket(co->_socketID);

    o["ServerResponse"] = PLAYERFORFEIT;

    d.setObject(o);
    *_byteBuffer = d.toJson();
    if (!sendAll(opponent))
        qDebug() << "Error sending forfeit to opponent: " << opponent;

    updateEngagedUsers(co->_username, findClientBySocket(opponent)->_username, false);

    //this gm is finished, kill it
    if (_gameMap->remove(co->_gameID) != 1)//pull the pointer out of the map
        qDebug() << "Somethin' weird happened here...";
    delete gm;
    gm = nullptr;
    //clean up this gm
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

//called when startGame happens
void TTTServer::updateEngagedUsers(QString challenger, QString challengee, bool gameStart)
{
    QJsonObject o;
    QJsonDocument d;
    QStringList users;
    QStringList status;

    users << challenger << challengee;

    if (gameStart)
        status << "true" << "true";
    else
        status << "false" << "false";

    QVariant var(users), var1(status);

    o["ServerResponse"] = UPDATEENGAGED;
    o["Users"] = QJsonValue::fromVariant(var);
    o["Status"] = QJsonValue::fromVariant(var1);

    d.setObject(o);
    *_byteBuffer = d.toJson();
    emit issueGlobalUpdate();
}

void TTTServer::makeMove(int clientSock, QJsonObject & obj)
{
    ClientObject * co = findClientBySocket(clientSock);

    //gotta for the map to use the right indirection
    GameManager * gm = _gameMap->operator [](co->_gameID);

    int row = obj["Row"].toInt();
    int col = obj["Col"].toInt();
    if (gm->makeMove(clientSock, row, col))
    {
        QJsonObject o;
        QJsonDocument d;
        int opponentSocket = gm->getOpponentSocket(clientSock);

        o["ServerResponse"] = RECEIVEMOVE;
        o["Username"] = co->_username;
        o["Row"] = row;
        o["Col"] = col;

        d.setObject(o);
        *_byteBuffer = d.toJson();
        qDebug() << "Client socket: " << clientSock << "\nOpponentSocket: " << opponentSocket;
        if (!sendAll(opponentSocket))
            qDebug() << "Error sending receive move to opponent...";


        if (gm->checkWinDrawCondition(clientSock))
        {
            o.remove("ServerResponse");
            o.remove("Username");
            o.remove("Row");
            o.remove("Col");

            qDebug() << "Found a gameover condition...";

            o["ServerResponse"] = GAMEOVER;
            if (gm->getState() == PLAYING)
                qDebug() << "Error, gameover didn't set right...";
            o["Reason"] = gm->getState();//should not ever be PLAYING

            //need to send to both clients
            d.setObject(o);
            *_byteBuffer = d.toJson();
            if (!sendAll(opponentSocket))
                qDebug() << "Error sending gameover to opponent...";
            *_byteBuffer = d.toJson();
            if (!sendAll(clientSock))
                qDebug() << "Error sending gameover to other opponent...";

            updateEngagedUsers(co->_username, findClientBySocket(opponentSocket)->_username, false);

            //this gm is finished, kill it
            if (_gameMap->remove(co->_gameID) != 1)//pull the pointer out of the map
                qDebug() << "Somethin' weird happened here...";
            delete gm;
            gm = nullptr;
            //clean up this gm

        }
    }
}

void TTTServer::declineInvite(int clientSock, QJsonObject & obj)
{//decliner is calling
    QString receiver = obj["Receiver"].toString();
    QJsonObject o;
    QJsonDocument d;

    o["ServerReponse"] = CHALLENGEDECLINED;
    o["ChallengeDeclineReason"] = USERDECLINE;
    d.setObject(o);
    *_byteBuffer = d.toJson();

    //we engage users by default on invite, must disengage if declining
    updateEngagedUsers(receiver, findClientBySocket(clientSock)->_username, false);

    if (!sendAll(findSocketByName(receiver)))
        qDebug() << "Error sending to client: " << receiver;
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

    d.setObject(o);
    *_byteBuffer = d.toJson();
    if (!sendAll(clientSock))
        qDebug() << "Error sending user list...";
}

void TTTServer::removeUser(int clientSock)
{
    ClientObject * user;
    if ((user = findClientBySocket(clientSock)) != nullptr)
    {//error socket not in list, should not occur

    }
    QJsonObject o;
    QJsonDocument d;

    o["ServerResponse"] = CLIENTLEFT;
    o["Username"] = user->_username;


    d.setObject(o);
    *_byteBuffer = d.toJson();

    qDebug() << user->_username << " has left the lobby...";
    emit issueGlobalUpdate();

    for (int i = 0; i < _clientList->size(); i++)
    {
        if (_clientList->at(i)->_socketID == clientSock)
            _clientList->removeAt(i);
    }
    //we previously grabbed pointer, removed from list
    delete user;//can now delete
    user = nullptr;
}

void TTTServer::inviteUser(int clientSock, QJsonObject & obj)
{
    //clientSock is challenger
    QString challenger = obj["Inviter"].toString();
    QString challengee = obj["Invitee"].toString();

    QJsonObject o;
    QJsonDocument d;

    o["ServerResponse"] = CHALLENGED;
    o["Challenger"] = challenger;
    d.setObject(o);

    int challengedUserSocket = findSocketByName(challengee);

    *_byteBuffer = d.toJson();
    if (!sendAll(challengedUserSocket))
    {
        qDebug() << "Error sending to client: " << challengee;
        //default action to say declined
        o.remove("ServerResponse");//clear challenged command
        o.remove("Challenger");//clear challenger item
        o["ServerReponse"] = CHALLENGEDECLINED;
        o["ChallengeDeclineReason"] = NETERROR;//let them know it was an error
        d.setObject(o);
        *_byteBuffer = d.toJson();
        if (!sendAll(clientSock))//notifying challenger
            qDebug() << "Error on the end of: " << challenger;
    }
}

void TTTServer::startGame(int clientSock, QJsonObject & obj)
{//clientSock is caller, responding to challenger
    QString receiver = obj["Receiver"].toString();
    static int gameIDHandler = 0;

    while (_gameMap->contains(gameIDHandler))//this loop will usually execute only once
        ++gameIDHandler;//increment our static id counter

    GameManager * gm = new GameManager(gameIDHandler, this);

    qDebug() << "Accepted invite, creating new game: " << gameIDHandler;

    _gameMap->insert(gameIDHandler, gm);

    //player "X" is the challenger
    int receiverSock = findSocketByName(receiver);
    gm->setXID(receiverSock);

    //player "O" is the invite accepter
    gm->setOID(clientSock);

    //assign the players their game id, so we can find it later
    ClientObject * temp = findClientBySocket(receiverSock);
    temp->_gameID = gameIDHandler;

    temp = findClientBySocket(clientSock);
    temp->_gameID = gameIDHandler;
    //end game id assignment

    qDebug() << "Before update all regarding engaged users in new game...";

    //this game started, let everyone know
    updateEngagedUsers(receiver, findClientBySocket(clientSock)->_username, true);

    qDebug() << "We told everyone these two are in a game now...";

    QJsonObject o;
    QJsonDocument d;

    o["ServerResponse"] = CHALLENGEACCEPTED;

    d.setObject(o);
    *_byteBuffer = d.toJson();

    //the accepter already knows they are playing, tell the challenger
    if (!sendAll(receiverSock))
        qDebug() << "Error sending accept challenge to user: " << receiver;
}

int TTTServer::findSocketByName(QString name)
{
    int found = -1;

    for (ClientObject * obj : *_clientList)
    {
        if (obj->_username == name)
        {
            found = obj->_socketID;
            break;
        }
    }
    return found;
}

ClientObject * TTTServer::findClientBySocket(int clientSock)
{
    ClientObject * found = nullptr;

    for (ClientObject * obj : *_clientList)
    {
        if (obj->_socketID == clientSock)
        {
            found = obj;
            break;
        }
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
