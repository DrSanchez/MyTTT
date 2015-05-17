#include "tttserver.h"
#include <QThreadPool>

//debug include
#include <QDebug>

TTTServer::TTTServer(QObject *parent)
    : QObject(parent), _byteBuffer(nullptr),
      _docObject(nullptr), _docHelper(nullptr), _clientList(nullptr),
      _gameMap(nullptr), _address(nullptr), _addressLength(0),
      _addressInfoPtr(NULL), _addressInfoList(NULL)
{
    for (int i = 0; i < BUFFER_MAX; i++)
        _basicBuffer[i] = '\0';
    _byteBuffer = new QByteArray();
    _docObject = new QJsonObject();
    _docHelper = new QJsonDocument();
    _clientList = new QList<ClientObject>();
    _gameMap = new QMap<int, GameManager>();

    //run the setup pre-listener
    setupServer();

    //setup listeners
    for (_addressInfoPtr = _addressInfoList; _addressInfoPtr != NULL; _addressInfoPtr = _addressInfoPtr->ai_next)
    {
        _listener = socket(_addressInfoPtr->ai_family, _addressInfoPtr->ai_socktype, _addressInfoPtr->ai_protocol);

        //this listener didnt work
        if (_listener < 0) {continue;}

        int yes = 1;//magic for following call....
        setsockopt(_listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(_listener, _addressInfoPtr->ai_addr, _addressInfoPtr->ai_addrlen) < 0)
        {//this listener didnt work
            close(_listener);
            continue;
        }

        break;
    }

    if (_addressInfoPtr == NULL)
    {
        perror("Server failed to bind...");
        exit(EXIT_FAILURE);
    }

    //no longer need this
    freeaddrinfo(_addressInfoList);

    //time to start initial listening
    if (listen(_listener, LISTEN_QUEUE) == -1)
    {
        perror("Error in listen...");
        exit(EXIT_FAILURE);
    }

    //add listener to master set of descriptors
    FD_SET(_listener, &_master);

    //largest descriptor
    _fdMax = _listener;

    //server listener prepared, begin server thread
    QThreadPool::globalInstance()->start(this);
}

TTTServer::~TTTServer()
{
}

void TTTServer::run()
{
    while(_running)
    {

    }
}

void TTTServer::setupServer()
{
    int hostLength = 0, errorVal = 0;
    struct addrinfo hint;
    char * buffHelper = nullptr;

    if ((hostLength = sysconf(_SC_HOST_NAME_MAX)) < 0)
        hostLength = DEF_HOSTNAME_MAX;

    if ((buffHelper = (char *)malloc(hostLength)) == NULL)
    {
        perror("Error on malloc...");
        exit(EXIT_FAILURE);
    }
    
    if (gethostname(buffHelper, hostLength) < 0)
    {
        perror("Error on gethostname...");
        exit(EXIT_FAILURE);
    }

    memset(&hint, 0, sizeof(hint));
    hint.ai_flags = AI_CANONNAME;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    
    if ((errorVal = getaddrinfo(buffHelper, "42040", &hint, &_addressInfoList)) != 0)
    {
        perror("Error on getaddrinfo...");
        exit(EXIT_FAILURE);
    }
}





