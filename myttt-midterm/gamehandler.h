#ifndef GAMEHANDLER_H
#define GAMEHANDLER_H

#include <QObject>
#include <QString>

enum GameState
{
    DRAW,
    WIN_X,
    WIN_Y,
    PLAYING
};

enum TileState
{
    OPEN,
    USED_X,
    USED_O
};

class GameHandler : public QObject
{
    Q_OBJECT
public:
    explicit GameHandler(QObject *parent = 0);

    bool validateMove(int row, int col, QString symbol);

    GameState getState();

    void setPlayerX(QString name);
    void setPlayerO(QString name);
    void setGameState(GameState state);

    QString getSymbolByPlayerName(QString name);

signals:
    //client will add the
    void notifyServerOfMove(int row, int col);

public slots:
    
private:
    //private data members
    QString     _playerXName;
    QString     _playerOName;
    GameState   _lastState;
    TileState   _boardView[3][3];

};

#endif // GAMEHANDLER_H
