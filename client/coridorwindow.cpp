#include "coridorwindow.h"
#include "ui_CoridorWindow.h"
#include <QMessageBox>

void Walls::update(QPoint pos)
{
	if (hSelected)
	{
		hX = pos.x() - 54;
		hY = pos.y() - 18;
	}
	if (vSelected)
	{
		vX = pos.x() - 18;
		vY = pos.y() - 54;
	}
	// qDebug() << gX << " " << gY;
	// qDebug() << vX << " " << vX;
}

void Walls::mousePressed(QPoint pos)
{
	if (pos.x() >= hX && pos.x() <= hX + 108 && pos.y() >= hY && pos.y() <= hY + 36)
	{
		hSelected = true;
	}
	if (pos.x() >= vX && pos.x() <= vX + 36 && pos.y() >= vY && pos.y() <= vY + 108)
	{
		vSelected = true;
	}
	update(pos);
}

void Walls::mouseMoved(QPoint pos, QPoint point)
{
	if (point.x() >= 0 && point.x() <= 16 && point.y() >= 0 && point.y() <= 16)
	{
		double cfx = 1.0 * 36 * 18 / 17.0;
		double cfy = 1.0 * 36 * 18 / 17.0;
		if (hSelected || vSelected)
		{
			pos.setX(point.x() * cfx + 18);
			pos.setY(point.y() * cfy + 18);
		}
	}
	update(pos);
}

void Walls::mouseReleased(QPoint pos)
{
	hX = 662;
	hY = 100;
	hSelected = false;
	vX = 700;
	vY = 150;
	vSelected = false;
	update(pos);
}

CoridorWindow::CoridorWindow(QString _firstPlayer, QString _secondPlayer, QString _player, QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::CoridorWindow)
	, player(_player)
{
	ui->setupUi(this);
	setWindowTitle("Coridor - " + _player);
	setMinimumWidth(784);
	setMinimumHeight(660);
	ui->restartBtn->hide();
	// connect(ui->pushButton_2, SIGNAL(pressed()), this, SLOT(start_pushButton_clicked()));
	connect(ui->exitBtn, SIGNAL(clicked()), this, SLOT(exitBtn_clicked()));

	pictures = new Images;
	pictures->load();

	field = new Field(pictures, 0, 0, 36 * 18, 36 * 18);
	game = new CoridorLogic(_firstPlayer, _secondPlayer);
	if (_player == _firstPlayer)
		status = "Move or place a wall";
	else
		status = "Waiting for another player";

	field->redrawCoridor(game->pole);
	show();
}

CoridorWindow::~CoridorWindow()
{
	delete ui;
}

void CoridorWindow::paintEvent(QPaintEvent* pEvent)
{
	QPainter painter(this);
	field->redrawCoridor(game->pole);
	ui->status->setText(status);
	ui->turn->setText("Turn of player " + game->getPlayerName(game->currentPlayerId));
	ui->wallsNumber_1->setText(game->player1.name + "'s walls: " + QString::number(game->coords[2][0]));
	ui->wallsNumber_2->setText(game->player2.name + "'s walls: " + QString::number(game->coords[2][1]));
	painter.drawImage(field->getX(), field->getY(), field->getImage());
	painter.drawImage(walls.hX, walls.hY, walls.gorWall);
	painter.drawImage(walls.vX, walls.vY, walls.verWall);
}

void CoridorWindow::placeWall(QPoint point, bool horizontal)
{
	if (horizontal)
	{
		try
		{
			if (game->nextTurn(game->placeWall(game->currentPlayerId, (point.x() - 1) / 2, (point.y() - 1) / 2, 'h')) != 0)
			{
				status = game->getPlayerName(game->winner) + " wins!";
			}
			else
				status = "Move or place a wall";
		}
		catch (const exception& e)
		{
			cout << e.what() << endl;
			status = e.what();
		}
	}
	else // if (walls.vSelected)
	{
		try
		{
			if (game->nextTurn(game->placeWall(game->currentPlayerId, (point.x() - 1) / 2, (point.y() - 1) / 2, 'v')) != 0)
			{
				status = game->getPlayerName(game->winner) + " wins!";
			}
			else
				status = "Move or place a wall";
		}
		catch (const exception& e)
		{
			cout << e.what() << endl;
			status = e.what();
		}
	}
}

void CoridorWindow::movePlayer(QPoint point)
{
	try
	{
		// qDebug() << point.x() << " " << point.y();
		if (game->nextTurn(game->move(game->pole, game->currentPlayerId, point.x(), point.y())) != 0)
		{
			status = game->getPlayerName(game->winner) + " wins!";
		}
		else
			status = "Move or place a wall";
	}
	catch (const exception& e)
	{
		cout << e.what() << endl;
		status = e.what();
	}
}

/*void CoridorWindow::nextTurn(int res)
{
	game->end = res;
	status = "Move or place a wall";
	if (game->end == 1)
		game->winner = game->player;
	else
		game->winner = (game->player + 1) % 2;
	game->player = (game->player + 1) % 2;
	game->output(game->pole);

	if (game->end != 0)
		end();
}*/

/*void CoridorWindow::end()
{
	status = "Player" + QString::number(game->winner + 1) + " wins!";
	switch (game->endValue)
	{
	case 1:
		cout << "Player" << game->winner + 1 << " wins!" << endl;
		break;
	case 2:
		cout << "Player" << game->winner + 1 << " wins!" << endl;
		// cout << "Ops, Player" << number << " closed himself!" << endl;
		break;
	}
}*/

void CoridorWindow::mousePressEvent(QMouseEvent* mEvent)
{
	if (game->endValue == 0)
	{
		QPoint pos = mEvent->pos();
		// QPoint point = field->getCoord(pos.x(), pos.y());
		walls.mousePressed(pos);

		// SelectPlayer(point);
		update();
	}
}

void CoridorWindow::mouseMoveEvent(QMouseEvent* mEvent)
{
	if (game->endValue == 0)
	{
		QPoint pos = mEvent->pos();
		QPoint point = field->getCoordCoridor(pos.x(), pos.y());
		walls.mouseMoved(pos, point);
		update();
	}
}

void CoridorWindow::mouseReleaseEvent(QMouseEvent* mEvent)
{
	if (game->endValue == 0)
	{
		QPoint pos = mEvent->pos();
		QPoint point = field->getCoordCoridor(pos.x(), pos.y());

		if (game->checkTurn(player))
		{
			if (walls.vSelected || walls.hSelected && checkPoint(point))
			{
				emit coridorSendQPointSignal(point, false, game->getPlayerName((game->currentPlayerId + 1) % 2), walls.hSelected);
				placeWall(point, walls.hSelected);
				if (status == "Move or place a wall")
					status = "Waiting for another player";
			}
			else if (checkPoint(point))
			{

				emit coridorSendQPointSignal(point, true, game->getPlayerName((game->currentPlayerId + 1) % 2), true);
				movePlayer(point);
				if (status == "Move or place a wall")
					status = "Waiting for another player";
			}
		}
		else
			status = "Its not your turn";
		walls.mouseReleased(pos);
		update();
		if (game->endValue != 0)
		{
			emit sendGameEndSignal();
			// QMessageBox::information(this, tr("End"), status + " you can leave now");
		}
	}
	else
		QMessageBox::information(this, tr("End"), status + " you can leave now");
}

void CoridorWindow::closeEvent(QCloseEvent* event)
{
	if (game->endValue == 0)
		emit sendGameEndSignal();
	emit firstWindow(); // И вызываем сигнал на открытие главного окна
	disconnect();
	// close(); // Закрываем окно
	deleteLater();
	event->accept();
}

void CoridorWindow::exitBtn_clicked()
{
	if (game->endValue == 0)
		emit sendGameEndSignal();
	emit firstWindow(); // И вызываем сигнал на открытие главного окна
	disconnect();
	close(); // Закрываем окно
	deleteLater();
}

void CoridorWindow::start_pushButton_clicked()
{
	// field = new Field(pictures, 0, 0, 36 * 18, 36 * 18);
	// game = new CoridorLogic();
	update();
}

void CoridorWindow::coridorRecieveQPoint(QPoint point, bool move, QString reciever, bool horizontal)
{
	if (move /*& checkPoint(point)*/)
		movePlayer(point);
	else // if (checkPoint(point))
		placeWall(point, horizontal);
	update();
	/*if (game->endValue != 0)
	{
		// emit sendDeleteLobbySignal();
		QMessageBox::information(this, tr("End"), status + " you can leave now");
	}*/
}

void CoridorWindow::gameEnd()
{
	if (game->endValue == 0)
	{
		game->endValue = 3;
		status = "Your opponet has left the game";
	}
	update();
	QMessageBox::information(this, tr("End"), status + " you can leave now");
}

/*void CoridorWindow::recieveQuit()
{
	game->endValue = 3;
	status = "Your opponet has left the game";
	update();
	QMessageBox::information(this, tr("End"), status + " you can leave now");
}*/

void CoridorWindow::closeGameSlot()
{
	close();
	deleteLater();
}

bool CoridorWindow::checkPoint(QPoint point)
{
	return point.x() >= 0 && point.x() <= 16 && point.y() >= 0 && point.y() <= 16;
}
