#ifndef GAME_H
#define GAME_H

#include <list>

enum GameType
{
	Coridor,
	Quarto,
	WrongGameType
};

class Game
{
public:
	Game(GameType type);
	GameType game;
};

#endif // GAME_H
