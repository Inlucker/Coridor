#ifndef CORIDOR_LOGIC_H
#define CORIDOR_LOGIC_H

#include <QString>
#include <assert.h>
#include <exception>
#include <iostream>
#include <map>
#include <queue>

using namespace std;

struct CoridorPlayer
{
	QString name;
	int id;
};

class CoridorLogic
{
public:
	CoridorLogic(QString firstPlayer, QString secondPlayer);
	const int size = 9;
	const int realSize = 2 * size - 1;
	const int w = 5;

	int pole[17][17];

	int** coords;

	int endValue = 0;
	int currentPlayerId = 0; // 0 - "первый", 1 - "второй"
	int winner = 0;			 // 0 - "первый", 1 - "второй"

	void output(int pole[17][17]);
	bool inPole(int x, int y);
	pair<int, int> inPole(pair<int, int> p);
	pair<int, int> step(int x, int y, char ch);
	bool isPlayer(int new_x, int new_y);
	int move(int pole[17][17], int player_id, int new_x, int new_y);
	int placeWall(int player_id, int x, int y, char dir);
	bool isWall(int x0, int x1, int y0, int y1);
	bool search(int x, int y, int p);
	int nextTurn(int res);
	bool checkDistanse(int x, int new_y, int player_id);
	bool jumpCheck(int new_x, int new_y, int player_id);

	CoridorPlayer player1, player2;

	QString getPlayerName(int currentId);
	bool checkTurn(QString playerName);
};

#endif // CORIDOR_LOGIC_H
