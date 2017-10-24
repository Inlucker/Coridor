//#include <QCoreApplication>
#include <iostream>
//#include <clocale>

using namespace std;

struct walls
{
	bool pos[8][8];
	bool hor[8][8];
	walls()
	{
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				pos[i][j] = false;
			}
		}
	}
};

bool checkWall(bool ver, int c1, int c2, int z, walls Walls);

bool checkPlaceWall(bool ver, int x, int y, walls Walls);

void placeWall(walls& Walls);

void drawField(const walls& Walls, int pole[9][9]);

void step(int pole[9][9], walls& Walls, bool& flag, int& end, int& wallsAmount, int& x, int& y, int player);

int main(/*int argc, char *argv[]*/)
{
	// QCoreApplication a(argc, argv);

	//для того чтобы читались русские буквы
	// using std::setlocale;

	// setlocale(LC_ALL, ""); //прописываем, для согласования кодировок

	int pole[9][9];				//Создаём поле
	for (int i = 0; i < 9; i++) //Заполняем нулями
	{
		for (int j = 0; j < 9; j++)
		{
			pole[i][j] = 0;
		}
	}

	//Ставим фишки на начальные места
	pole[0][4] = 1;
	int x1 = 0;
	int y1 = 4;
	pole[8][4] = 2;
	int x2 = 8;
	int y2 = 4;

	//Храним колличество стенок каждого игрока
	int wallsAmount1 = 10;
	int wallsAmount2 = 10;
	char tmp = '0';

	//Создём данные хранящие положения стенок
	walls Walls;

	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			cout << pole[j][i] << "   ";
		}
		cout << "\n\n";
	}

	//Храним информации о конце игры
	int end = 0;

	bool flag = true;

	while (end == 0)
	{
		// Player1 ходит
		step(pole, Walls, flag, end, wallsAmount1, x1, y1, 1);

		if (end == 1)
			break; //Если Player1 победил

		flag = true;

		drawField(Walls, pole);

		// Player2 ходит
		step(pole, Walls, flag, end, wallsAmount2, x2, y2, 2);

		flag = true;

		drawField(Walls, pole);
	}

	cout << "Player" << end << " wins!" << endl;

	return 0;
}

void placeWall(walls& Walls)
{
	int x, y;
	cout << "Chose x and y to place a wall (x y): ";
	cin >> x >> y;
	cout << '\n';

	if (Walls.pos[x][y] == true || x < 0 || x > 7 || y < 0 || y > 7)
	{
		cout << "Error: wrong x or/and y!\n\n";
		placeWall(Walls);
		return;
	}

	char dir;

	while (true)
	{
		cout << "Chose direction of the wall (v/h): ";
		cin >> dir;
		cout << '\n';

		switch (dir)
		{
		case 'v':
			if (!checkPlaceWall(true, x, y, Walls))
			{
				cout << "Error: you can't place wall like that!\n";
				placeWall(Walls);
				return;
			}
			break;
		case 'h':
			if (!checkPlaceWall(false, x, y, Walls))
			{
				cout << "Error: you can't place wall like that!\n";
				placeWall(Walls);
				return;
			}
			break;
		default:
			break;
		}

		switch (dir)
		{
		case 'v':
			Walls.pos[x][y] = true;
			Walls.hor[x][y] = false;
			break;
		case 'h':
			Walls.pos[x][y] = true;
			Walls.hor[x][y] = true;
			break;
		default:
			cout << "Error: wrong char!\n";
			break;
		}
		if (dir == 'v' || dir == 'h')
			break;
	}
}

bool checkWall(bool ver, int c1, int c2, int z, walls Walls)
{
	int c = 0;
	if (c1 < c2)
		c = c1;
	else
		c = c2;
	if (!ver)
	{
		if (z != 0 && z != 8)
		{
			if (Walls.pos[c][z - 1] && !Walls.hor[c][z - 1] || Walls.pos[c][z] && !Walls.hor[c][z])
				return false;
			return true;
		}
		else if (z == 0)
		{
			if (Walls.pos[c][z] && !Walls.hor[c][z])
				return false;
			return true;
		}
		else
		{
			if (Walls.pos[c][z - 1] && !Walls.hor[c][z - 1])
				return false;
			return true;
		}
	}
	else
	{
		if (z != 0 && z != 8)
		{
			if (Walls.pos[z - 1][c] && Walls.hor[z - 1][c] || Walls.pos[z][c] && Walls.hor[z][c])
				return false;
			return true;
		}
		else if (z == 0)
		{
			if (Walls.pos[z][c] && Walls.hor[z][c])
				return false;
			return true;
		}
		else
		{
			if (Walls.pos[z - 1][c] && Walls.hor[z - 1][c])
				return false;
			return true;
		}
	}
}

bool checkPlaceWall(bool ver, int x, int y, walls Walls)
{
	if (!ver)
	{
		if (x != 0)
			if (Walls.pos[x - 1][y] == true && Walls.hor[x - 1][y])
				return false;
		if (x != 7)
			if (Walls.pos[x + 1][y] == true && Walls.hor[x + 1][y])
				return false;
	}
	else
	{
		if (y != 0)
			if (Walls.pos[x][y - 1] == true && !Walls.hor[x][y - 1])
				return false;
		if (y != 7)
			if (Walls.pos[x][y + 1] == true && !Walls.hor[x][y + 1])
				return false;
	}
	return true;
}

void drawField(const walls& Walls, int pole[9][9])
{
	for (int i = 0; i < 9; i++) //Вывод поля
	{
		for (int j = 0; j < 9; j++)
		{
			cout << pole[j][i] << " ";
			if (j != 8)
			{
				if (i != 0 && i != 8)
				{
					if (Walls.pos[j][i] && !Walls.hor[j][i] || Walls.pos[j][i - 1] && !Walls.hor[j][i - 1])
						cout << "| ";
					else
						cout << "  ";
				}
				else if (i == 0)
				{
					if (Walls.pos[j][i] && !Walls.hor[j][i])
						cout << "| ";
					else
						cout << "  ";
				}
				else
				{
					if (Walls.pos[j][i - 1] && !Walls.hor[j][i - 1])
						cout << "| ";
					else
						cout << "  ";
				}
			}
		}
		cout << "\n";
		for (int j = 0; j < 8; j++)
		{
			if (j != 0 && j != 8)
			{
				if (Walls.pos[j][i] && Walls.hor[j][i] || Walls.pos[j - 1][i] && Walls.hor[j - 1][i])
					cout << "-   ";
				else
					cout << "   ";
			}
			else if (j == 0)
			{
				if (Walls.pos[j][i] && Walls.hor[j][i])
					cout << "-   ";
				else
					cout << "   ";
			}
			else
			{
				if (Walls.pos[j - 1][i] && Walls.hor[j - 1][i])
					cout << "-   ";
				else
					cout << "   ";
			}
		}
		cout << "\n";
	}
}

void step(int pole[9][9], walls& Walls, bool& flag, int& end, int& wallsAmount, int& x, int& y, int player)
{
	while (flag)
	{
		cout << "Turn of Player" << player << ", chose move or place a wall (m/w): ";
		char tmp;
		cin >> tmp;
		cout << '\n';
		switch (tmp)
		{
		case 'm':
			while (flag)
			{
				cout << "Chose direction - left, right, down or up (l/r/d/u): ";
				cin >> tmp;
				cout << '\n';
				switch (tmp)
				{

				case 'l':
					if (x != 0 && checkWall(false, x, x - 1, y, Walls))
					{
						pole[x - 1][y] = player;
						pole[x][y] = 0;
						x--;
						flag = false;
					}
					else
						cout << "Error: you cant move left!\n";
					break;
				case 'r':
					if (x != 8 && checkWall(false, x, x + 1, y, Walls))
					{
						pole[x + 1][y] = player;
						pole[x][y] = 0;
						x++;
						flag = false;
					}
					else
						cout << "Error: you cant move right!\n";
					break;
				case 'd':
					if (y != 8 && checkWall(true, y, y + 1, x, Walls))
					{
						pole[x][y + 1] = player;
						pole[x][y] = 0;
						y++;
						flag = false;
					}
					else
						cout << "Error: you cant move up!\n";
					break;
				case 'u':
					if (y != 0 && checkWall(true, y, y - 1, x, Walls))
					{
						pole[x][y - 1] = player;
						pole[x][y] = 0;
						y--;
						flag = false;
					}
					else
						cout << "Error: you cant move down!\n";
					break;
				default:
					cout << "Error: wrong char!\n";
					break;
				}
			}
			for (int j = 0; j < 9; j++)
				if (pole[8][j] == 1)
					end = 1; //Проверка на победу
			break;
		case 'w':
			if (wallsAmount == 0)
			{
				cout << "Error: Player1 out of walls!\n";
				break;
			}
			placeWall(Walls);
			flag = false;
			break;
		default:
			cout << "Error: wrong char!\n";
			break;
		}
	}
}
