#ifndef CORIDORWINDOW_H
#define CORIDORWINDOW_H

#include "field.h"
#include "images.h"
#include "player.h"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QImage>
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QString>
#include <QWidget>

namespace Ui
{
	class CoridorWindow;
}

struct Walls
{
	QImage gorWall = QImage("PICTURES/GorWall.png");
	QImage verWall = QImage("PICTURES/VerWall.png");
	int hX = 662, hY = 100, vX = 700, vY = 150;
	bool hSelected = false, vSelected = false;
	void update(QPoint pos);
	void mousePressed(QPoint pos);
	void mouseMoved(QPoint pos, QPoint point);
	void mouseReleased(QPoint pos);
};

class CoridorWindow : public QWidget
{
	Q_OBJECT

public:
	explicit CoridorWindow(QWidget* parent = 0);
	~CoridorWindow();

protected:
	void paintEvent(QPaintEvent* pEvent);
	void mousePressEvent(QMouseEvent* mEvent);
	void mouseMoveEvent(QMouseEvent* mEvent);
	void mouseReleaseEvent(QMouseEvent* mEvent);

signals:
	void firstWindow(); // Сигнал для первого окна на открытие

private slots:
	// Слот-обработчик нажатия кнопки
	void on_pushButton_clicked();
	void start_pushButton_clicked();

private:
	Ui::CoridorWindow* ui;
	// QPixmap *spriteImage;
	Images* pictures;
	QImage* image;
	Field* field;
	QGraphicsScene* scene;
	Walls walls;
	GameLogic* game;
	QString status = "Move or place a wall";
	// bool pl1Sel, pl2Sel;

	// void SelectPlayer(QPoint point);
	void PlaceWall(QPoint point);
	void MovePlayer(QPoint point);
	void nextTurn(int res);
	void end();
};

#endif // CORIDORWINDOW_H
