#include "tttuser.h"

TTTUser::TTTUser(QObject *parent)
    : QObject(parent)
{
}

int TTTUser::id()
{
    return _id;
}

int TTTUser::gameId()
{
    return _gameID;
}

QString TTTUser::username()
{
    return _username;
}

ClientState TTTUser::state()
{
    return _state;
}

void TTTUser::setId(int id)
{
    if (_id != id)
    {
        _id = id;
        emit idChanged();
    }
}

void TTTUser::setGameId(int id)
{
    if (_gameID != id)
    {
        _gameID = id;
        emit gameIdChanged();
    }
}

void TTTUser::setUsername(QString name)
{
    if (_username != name)
    {
        _username = name;
        emit usernameChanged();
    }
}

void TTTUser::setClientState(ClientState state)
{
    if (_state != state)
    {
        _state = state;
        emit stateChanged();
    }
}
