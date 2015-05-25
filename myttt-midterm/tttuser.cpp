#include "tttuser.h"

TTTUser::TTTUser(QObject *parent)
    : QObject(parent)
{
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

QString TTTUser::piece()
{
    return _piece;
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

void TTTUser::setPiece(QString piece)
{
    if (_piece != piece)
    {
        if (piece == "X" || piece == "O")
        {
            _piece = piece;
            emit pieceChanged();
        }
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
