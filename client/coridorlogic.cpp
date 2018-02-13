#include "coridorlogic.h"

#include <QDebug>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

CoridorLogic::CoridorLogic(QString firstPlayer, QString secondPlayer)
{
	player1.id = 0;
	player1.name = firstPlayer;
	player2.id = 1;
	player2.name = secondPlayer;

	for (int i = 0; i < realSize; i++) //Заполняем -1
	{
		for (int j = 0; j < realSize; j++)
		{
			if (j % 2 == 1 || i % 2 == 1)
				pole[i][j] = -2;
			else
				pole[i][j] = -1;
		}
	}

	pole[0][8] = 0;
	pole[16][8] = 1;

	int mas[3][2];
	for (int i = 0; i < 2; i++)
	{
		mas[0][i] = 16 * i;
		mas[1][i] = 8;
		mas[2][i] = 10;
	}

	coords = (int**)malloc(3 * 2 * sizeof(int*));

	for (int i = 0; i < 3; i++)
		coords[i] = (int*)malloc(3 * 2 * sizeof(int));
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 2; j++)
			coords[i][j] = mas[i][j];
}

void CoridorLogic::output(int pole[17][17])
{
	for (int i = 0; i < realSize; i++) //Вывод поля
	{
		for (int j = 0; j < realSize; j++)
		{
			if (pole[j][i] == -1 && j % 2 == 0 && i % 2 == 0)
				cout << "0 ";
			else if (pole[j][i] == 0)
				cout << "1 ";
			else if (pole[j][i] == 1)
				cout << "2 ";
			else if (pole[j][i] == w)
				if (j % 2 == 1 && i % 2 == 1)
					cout << "  ";
				else if (i % 2 == 0)
					cout << "| ";
				else
					cout << "- ";
			else
				cout << "  ";
		}
		cout << endl;
	}
	cout << endl;
}

bool CoridorLogic::inPole(int x, int y)
{
	return x >= 0 && x < realSize && y >= 0 && y < realSize;
}

pair<int, int> CoridorLogic::inPole(pair<int, int> p)
{
	if (!inPole(p.first, p.second))
		throw runtime_error("Can't move here");
	return p;
}

pair<int, int> CoridorLogic::step(int x, int y, char ch)
{
	switch (ch)
	{
	case 'l':
		return inPole(make_pair(x - 2, y));
	case 'r':
		return inPole(make_pair(x + 2, y));
	case 'd':
		return inPole(make_pair(x, y + 2));
	case 'u':
		return inPole(make_pair(x, y - 2));
	default:
		return inPole(make_pair(-1, -1));
	}
}

bool CoridorLogic::isPlayer(int new_x, int new_y)
{
	for (int i = 0; i < 2; i++)
	{
		if (new_x == coords[0][i] && new_y == coords[1][i])
			return true;
	}
	return false;
}

int CoridorLogic::move(int pole[17][17], int player_id, int new_x, int new_y)
{
	cout << "Chose direction - left, right, down or up (l/r/d/u): ";
	// char ans;
	// cin >> ans;
	cout << endl;
	// if (ans != 'l' && ans != 'r' && ans != 'd' && ans != 'u')
	// throw runtime_error("Error: wrong char!");

	// pair<int, int> new_coords = step(coords[0][player_id], coords[1][player_id], ans, 2);
	// int new_x = new_coords.first;
	// int new_y = new_coords.second;

	if (isWall(coords[0][player_id], coords[1][player_id], new_x, new_y))
		throw runtime_error("Error: you cant move here (walls)");

	if (checkDistanse(new_x, new_y, player_id) && !isPlayer(new_x, new_y))
	{
		swap(pole[new_x][new_y], pole[coords[0][player_id]][coords[1][player_id]]);
		coords[0][player_id] = new_x;
		coords[1][player_id] = new_y;
	}
	else
	{
		if (jumpCheck(new_x, new_y, player_id))
		{
			swap(pole[new_x][new_y], pole[coords[0][player_id]][coords[1][player_id]]);
			coords[0][player_id] = new_x;
			coords[1][player_id] = new_y;
		}
		else
		{
			throw runtime_error("Wrong place to move");
		}
	}

	if (coords[0][player_id] == 16 - player_id * 16)
		return 1; //Проверка на победу
	else
		return 0;
}

int CoridorLogic::placeWall(int player_id, int x, int y, char dir)
{
	if (coords[2][player_id] == 0)
		throw runtime_error("Error: Player out of walls!");

	int t = coords[0][1 - player_id];
	int u = coords[1][1 - player_id];

	// int x, y;
	// char dir;
	cout << "Chose x, y and direction of the wall (x, y, v/h): ";
	// cin >> x >> y >> dir;
	cout << endl;
	cout << x << " " << y << endl;

	x = 2 * x + 1;
	y = 2 * y + 1;

	if (!inPole(x, y))
		throw runtime_error("Error: wrong x or/and y!");

	int wall[3][2];

	switch (dir)
	{
	case 'h':
		for (int i = 0; i < 3; ++i)
		{
			wall[i][0] = x + i - 1;
			wall[i][1] = y;
		}
		break;
	case 'v':
		for (int i = 0; i < 3; ++i)
		{
			wall[i][0] = x;
			wall[i][1] = y + i - 1;
		}
		break;
	default:
		throw runtime_error("Error: wrong char!");
	}

	for (int i = 0; i < 3; i++)
	{
		if (isWall(wall[i][0], wall[i][1], wall[i][0], wall[i][1]))
			throw runtime_error("Error: wrong x/y/dir!");
	}

	for (int i = 0; i < 3; i++)
	{
		pole[wall[i][0]][wall[i][1]] = w;
	}

	if (!search(t, u, 1 - player_id))
	{
		for (int i = 0; i < 3; i++)
		{
			pole[wall[i][0]][wall[i][1]] = -2;
		}
		throw runtime_error("Error: you can't place wall like that (you are blocking the way)!");
	}

	--coords[2][player_id];
	if (!search(coords[0][player_id], coords[1][player_id], player_id))
	{
		return 2;
	}
	return 0;
}

bool CoridorLogic::isWall(int x0, int y0, int x1, int y1)
{
	return pole[(x0 + x1) / 2][(y0 + y1) / 2] == w;
}

bool CoridorLogic::search(int x, int y, int p)
{
	bool field[9][9];

	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			field[i][j] = false;
		}
	}

	queue<pair<int, int>> frontier;
	frontier.push(make_pair(x / 2, y / 2));
	field[x / 2][y / 2] = true;
	pair<int, int> current;
	pair<int, int> next;

	while (!frontier.empty())
	{
		current = frontier.front();

		x = current.first;
		y = current.second;

		char dir;
		for (int i = 0; i < 4; ++i)
		{
			switch (i)
			{
			case (0):
				dir = 'l';
				break;
			case (1):
				dir = 'u';
				break;
			case (2):
				dir = 'r';
				break;
			case (3):
				dir = 'd';
				break;
			default:
				assert(false);
				break;
			}
			try
			{
				next = step(x * 2, y * 2, dir);
				if (!field[next.first / 2][next.second / 2] && !isWall(x * 2, y * 2, next.first, next.second) && next.first >= 0)
				{
					frontier.push(make_pair(next.first / 2, next.second / 2));
					field[next.first / 2][next.second / 2] = true;
				}
			}
			catch (exception e)
			{
				// Do nothing
			}
		}

		frontier.pop();

		for (int j = 0; j < 9; j++)
			if (field[8 - 8 * p][j])
			{
				return true;
			}
	}
	return false;
}

int CoridorLogic::nextTurn(int res)
{
	endValue = res;
	if (endValue == 1)
		winner = currentPlayerId;
	else
		winner = (currentPlayerId + 1) % 2;
	currentPlayerId = (currentPlayerId + 1) % 2;
	// output(pole);

	/*if (endValue!=0)
		end();*/
	return endValue;
}

bool CoridorLogic::checkDistanse(int new_x, int new_y, int player_id)
{
	if (abs(new_x - coords[0][player_id]) == 2 && new_y - coords[1][player_id] == 0 || new_x - coords[0][player_id] == 0 && abs(new_y - coords[1][player_id]) == 2)
		return true;
	else
		return false;
}

bool CoridorLogic::jumpCheck(int new_x, int new_y, int player_id)
{
	int other_id = (player_id + 1) % 2;
	int other_x = coords[0][other_id];
	int other_y = coords[1][other_id];
	int a = other_x + other_x - coords[0][player_id];
	int b = other_y + other_y - coords[1][player_id];

	if (checkDistanse(other_x, other_y, player_id) && !isWall(other_x, other_y, coords[0][player_id], coords[1][player_id]) && checkDistanse(new_x, new_y, other_id) && !isWall(new_x, new_y, other_x, other_y))
		if ((new_x == a && new_y == b && inPole(a, b)) || isWall(a, b, other_x, other_y) || !inPole(a, b))
			return true;
	return false;
}

QString CoridorLogic::getPlayerName(int currentId)
{
	if (currentId == player1.id)
		return player1.name;
	else if (currentId == player2.id)
		return player2.name;
}

bool CoridorLogic::checkTurn(QString playerName)
{
	return playerName == getPlayerName(currentPlayerId);
}
