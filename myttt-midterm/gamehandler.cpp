#include "gamehandler.h"
#include <QDebug>
GameHandler::GameHandler(QObject *parent)
    : QObject(parent)
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
    }
    else
        result = false;

    return result;
}
