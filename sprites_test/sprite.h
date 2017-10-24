#ifndef SPRITE_H
#define SPRITE_H

#include <QGraphicsItem>
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QTimer>

class Sprite : public QObject, public QGraphicsItem
{
	Q_OBJECT
public:
	explicit Sprite(QObject* parent = 0);

signals:

public slots:

private slots:
	void nextFrame(); // Слот для пролистывания изображения в QPixmap

private:
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
	QRectF boundingRect() const;

private:
	QTimer* timer;		  // Таймер для пролистывания изображения в QPixmap
	QPixmap* spriteImage; // В данный объект QPixamp будет помещён спрайт
	int currentFrame; // Координата X, с которой начинает очередной кадр спрайта
};

#endif // SPRITE_H
