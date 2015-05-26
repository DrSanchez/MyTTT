#include "tttuser.h"

TTTUser::TTTUser(QObject *parent)
    : QObject(parent)
{
}

TTTUser::~TTTUser()
{
    _username = "";
    _state = DISCONNECTING;
    _piece = "";
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
