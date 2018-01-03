#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

class Player
{
public:
    Player(QString login, int socketDescriptor);

    QString playerName;
    int ID, wins, loses;
};

#endif // PLAYER_H