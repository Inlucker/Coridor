#include "quartowindow.h"
#include "ui_quartowindow.h"
#include <QDebug>
#include <QMessageBox>

void Figures::redrawFiguresImage(Images pictures)
{
	figuresImage->fill(0);
	QPainter painter(figuresImage);
	double cfx = 1.0 * 36 * 5.0 / 4.0;
	double cfy = 1.0 * 36 * 5.0 / 4.0;
	for (int i = 0; i < 16; i++)
	{
		if (figures[i] && !fSelected[i])
			painter.drawImage(i % 4 * cfx, i / 4 * cfy, pictures.getImage(i));
	}
}

int Figures::getSelectedID()
{
	for (int i = 0; i < 16; i++)
	{
		if (fSelected[i])
		{
			return i;
		}
	}
	return -1;
}

void Figures::update(QPoint pos)
{
	selectedFigure = QImage("PICTURES/figure" + QString::number(getSelectedID()) + ".png");
	sX = pos.x() - 18;
	sY = pos.y() - 18;
}

void Figures::mousePressed(QPoint pos)
{
	if (pos.x() >= fX && pos.x() <= fX + 36 * 5 && pos.y() >= fY && pos.y() <= fY + 36 * 5)
	{
		double cfx = 1.0 * 36 * 5.0 / 4.0;
		double cfy = 1.0 * 36 * 5.0 / 4.0;

		if (figures[int((pos.x() - fX) / cfx) % 4 + int((pos.y() - fY) / cfy) % 4 * 4])
		{
			fSelected[int((pos.x() - fX) / cfx) % 4 + int((pos.y() - fY) / cfy) % 4 * 4] = true;
			figures[int((pos.x() - fX) / cfx) % 4 + int((pos.y() - fY) / cfy) % 4 * 4] = false;
		}
	}
	update(pos);
}

void Figures::mouseMoved(QPoint pos, QPoint point)
{
	if (point.x() >= 0 && point.x() <= 4 && point.y() >= 0 && point.y() <= 4)
	{
		double cfx = 1.0 * 36 * 5 / 4.0;
		double cfy = 1.0 * 36 * 5 / 4.0;
		pos.setX(point.x() * cfx + 18);
		pos.setY(point.y() * cfy + 18);
	}
	update(pos);
}

QuartoWindow::QuartoWindow(QString _firstPlayer, QString _secondPlayer, QString _player, QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::QuartoWindow)
	, player(_player)
{
	ui->setupUi(this);
	ui->start->hide();
	setWindowTitle("Quarto - " + _player);
	setMinimumWidth(610);
	setMinimumHeight(329);
	// connect(ui->pushButton_2, SIGNAL(pressed()), this, SLOT(start_pushButton_clicked()));

	pictures = new Images;
	pictures->load2();

	field = new Field(pictures, 0, 0, 36 * 5, 36 * 5);

	for (int i = 0; i < 16; i++)
	{
		figures.figures[i] = true;
		figures.fSelected[i] = false;
	}
	figures.figuresImage = new QImage(36 * 5, 36 * 5, QImage::Format_ARGB32);

	game = new QuartoLogic(_firstPlayer, _secondPlayer);

	if (_player == _firstPlayer)
		status = "Place a figure";
	else
		status = "Waiting for another player";

	figures.redrawFiguresImage(*pictures);
	field->redrawQuarto(game->pole);
	show();
}

QuartoWindow::~QuartoWindow()
{
	delete ui;
}

void QuartoWindow::paintEvent(QPaintEvent* pEvent)
{
	QPainter painter(this);
	field->redrawQuarto(game->pole);
	ui->status->setText(status);
	ui->turn->setText("Turn of  " + game->getPlayerName(game->currentPlayerId));
	painter.drawImage(field->getX(), field->getY(), field->getImage());
	figures.redrawFiguresImage(*pictures);
	painter.drawImage(figures.fX, figures.fY, *figures.figuresImage);
	if (figures.getSelectedID() != -1)
		painter.drawImage(figures.sX, figures.sY, figures.selectedFigure);
}

void QuartoWindow::placeFigure(QPoint point, int figureId)
{
	if (game->placeFigure(point.x(), point.y(), figureId))
		figures.figures[figureId] = true;
	else
		figures.figures[figureId] = false;
	figures.fSelected[figureId] = false;
	update();
}

void QuartoWindow::checkWin()
{
	game->checkWin();
	status = game->getPlayerName(game->winner) + " wins!";
}

void QuartoWindow::nextTurn()
{
	game->nextTurn();
	status = "Place a figure";
}

void QuartoWindow::mousePressEvent(QMouseEvent* mEvent)
{
	QPoint pos = mEvent->pos();
	if (!game->end)
		figures.mousePressed(pos);

	update();
}

void QuartoWindow::mouseMoveEvent(QMouseEvent* mEvent)
{
	QPoint pos = mEvent->pos();
	QPoint point = field->getCoordQuarto(pos.x(), pos.y());
	figures.mouseMoved(pos, point);

	update();
}

void QuartoWindow::mouseReleaseEvent(QMouseEvent* mEvent)
{
	if (!game->end)
	{
		QPoint pos = mEvent->pos();
		QPoint point = field->getCoordQuarto(pos.x(), pos.y());

		// can this be oprimised??
		if (checkPoint(point))
		{
			if (figures.getSelectedID() != -1)
			{
				if (game->checkTurn(player))
				{
					emit sendQPointSignal(point, figures.getSelectedID(), game->getPlayerName((game->currentPlayerId + 1) % 2));
					placeFigure(point, figures.getSelectedID());
				}
				else
				{
					figures.figures[figures.getSelectedID()] = true;
					figures.fSelected[figures.getSelectedID()] = false;
					status = "Its not your turn";
				}
			}
			else
			{
				if (game->checkTurn(player))
				{
					status = "Grab a figure first";
				}
			}
		}
		else
		{
			if (figures.getSelectedID() != -1)
			{
				figures.figures[figures.getSelectedID()] = true;
				figures.fSelected[figures.getSelectedID()] = false;
			}
		}
		update();
	}
	else
		QMessageBox::information(this, tr("End"), status + " you can leave now");
}

void QuartoWindow::closeEvent(QCloseEvent* event)
{
	if (!game->end)
		emit sendGameEndSignal();
	emit firstWindow(); // И вызываем сигнал на открытие главного окна
	disconnect();
	// close(); // Закрываем окно
	deleteLater();
	event->accept();
}

void QuartoWindow::on_pushButton_clicked()
{
	if (!game->end)
		emit sendGameEndSignal();
	emit firstWindow(); // И вызываем сигнал на открытие главного окна
	disconnect();
	close(); // Закрываем окно
	deleteLater();
}

void QuartoWindow::on_checkWin_clicked()
{
	if (!game->end)
	{
		if (game->checkTurn(player))
		{
			emit quartoSendCheckWinSignal(game->getPlayerName((game->currentPlayerId + 1) % 2), true); // true
			checkWin();
		}
		else
			status = "Its not your turn";
		update();
		if (game->end)
		{
			emit sendGameEndSignal();
			// QMessageBox::information(this, tr("End"), status + " you can leave now");
		}
	}
	else
		QMessageBox::information(this, tr("End"), status + " you can leave now");
}

void QuartoWindow::on_start_clicked()
{
	// field = new Field(pictures, 0, 0, 36 * 5, 36 * 5);

	/*for (int i = 0; i < 16; i++)
	{
		figures.figures[i] = true;
		figures.fSelected[i] = false;
	}*/

	// game = new QuartoLogic();

	update();
}

void QuartoWindow::on_nextTurn_clicked()
{
	if (!game->end)
	{
		if (game->checkTurn(player))
		{
			emit quartoSendCheckWinSignal(game->getPlayerName((game->currentPlayerId + 1) % 2), false); // false
			nextTurn();
			status = "Waiting for another player";
		}
		else
			status = "Its not your turn";
		update();
	}
	else
		QMessageBox::information(this, tr("End"), status + " you can leave now");
}

void QuartoWindow::quartoRecieveQPoint(QPoint point, int figureId, QString reciever)
{
	placeFigure(point, figureId);
}

void QuartoWindow::quartoRecieveCheckWin(QString _enemy, bool _checkWin)
{
	if (_checkWin)
		checkWin();
	else
		nextTurn();
	update();
}

/*void QuartoWindow::recieveQuit()
{
	game->end = true;
	status = "Your opponet has left the game";
	update();
	QMessageBox::information(this, tr("End"), status + " you can leave now");
}*/

void QuartoWindow::closeGameSlot()
{
	close();
	deleteLater();
}

void QuartoWindow::gameEnd()
{
	if (!game->end)
	{
		game->end = true;
		status = "Your opponet has left the game";
	}
	update();
	QMessageBox::information(this, tr("End"), status + " you can leave now");
}

bool QuartoWindow::checkPoint(QPoint point)
{
	return point.x() >= 0 && point.x() <= 3 && point.y() >= 0 && point.y() <= 3;
}
