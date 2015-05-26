#include "gamehandler.h"
#include <QDebug>
GameHandler::GameHandler(QObject *parent)
    : QObject(parent)
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            _boardView[i][j] = OPEN;
}

void GameHandler::resetBoard()
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            _boardView[i][j] = OPEN;
}

bool GameHandler::validateMove(int row, int col, QString symbol)
{
    bool result = false;

    if (_boardView[row][col] == OPEN)
    {
        result = true;
        _boardView[row][col] = (symbol.contains("X") ? USED_X : USED_O);
        emit notifyServerOfMove(row, col);
    }
    else
        result = false;

    return result;
}

bool GameHandler::makeReceiverMove(int row, int col, QString symbol)
{
    bool result = false;

    if (_boardView[row][col] == OPEN)
    {
        result = true;
        _boardView[row][col] = (symbol.contains("X") ? USED_X : USED_O);
    }
    else
        result = false;

    return result;
}

GameState GameHandler::getState()
{
    return _lastState;
}

void GameHandler::setPlayerX(QString name)
{
    _playerXName = name;
}

void GameHandler::setPlayerO(QString name)
{
    _playerOName = name;
}

void GameHandler::setGameState(GameState state)
{
    _lastState = state;
}

QString GameHandler::getSymbolByPlayerName(QString name)
{
    if (name == _playerXName)
        return "X";
    else if (name == _playerOName)
        return "O";
    else
        return "";//this won't happen, makes compiler happy
}
