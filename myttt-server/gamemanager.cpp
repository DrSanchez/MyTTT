#include "gamemanager.h"
#include <QDebug>

GameManager::GameManager(int gameID, QObject *parent)
    : QObject(parent), _gameID(gameID)
{
    //initialize the board
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            _board[i][j] = OPEN;

    //initial game state when not playing
    _state = PLAYING;

}

int GameManager::getOpponentSocket(int socket)
{
    return (socket == _playerXID ? _playerOID : _playerXID);
}

void GameManager::setXID(int socket)
{
    _playerXID = socket;
}

void GameManager::setOID(int socket)
{
    _playerOID = socket;
}

bool GameManager::anyOpen()
{
    bool anyOpen = false;

    for (int i = 0; i < 3 && !anyOpen; i++)
        for (int j = 0; j < 3 && !anyOpen; j++)
            if (_board[i][j] == OPEN)
                anyOpen = true;

    return anyOpen;
}

GameState GameManager::getState()
{
    return _state;
}

bool GameManager::gameOverState()
{
    return (_state == WIN_X || _state == WIN_O || _state == DRAW);
}

bool GameManager::checkWinDrawCondition(int player)
{
    if (player != _playerXID && player != _playerOID)
    {//error wrong game, this probably will not happen
        qDebug() << "Error, asked the wrong game about player: " << player;
        emit wrongGameError(player, _gameID);
        return false;
    }

    //check diagonal
    if((_board[1][1] != OPEN) &&
       ((_board[0][0] == _board[1][1] && _board[0][0] == _board[2][2]) ||
       (_board[0][2] == _board[1][1] && _board[0][2] == _board[2][0])))
        _state = (player == _playerXID ? WIN_X : WIN_O);

    //check row/col
    for (int i = 0; i < 3 && !gameOverState(); i++)
        if((_board[i][i] != OPEN) &&
           ((_board[i][0] == _board[i][1] && _board[i][0] == _board[i][2])||
           (_board[0][i] == _board[1][i] && _board[0][i] == _board[2][i])))
            _state = (player == _playerXID ? WIN_X : WIN_O);

    if (!anyOpen())
        _state = DRAW;

    return (_state == WIN_X || _state == WIN_O || _state == DRAW);
}

bool GameManager::makeMove(int player, int row, int col)
{
    bool valid = false;
    if (_board[row][col] == OPEN)
    {
        _board[row][col] = (player == _playerXID ? USED_X : USED_O);
        valid = true;
    }

    return valid;
}
