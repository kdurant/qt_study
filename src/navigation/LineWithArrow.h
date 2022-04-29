#ifndef LINEWITHARROW_H
#define LINEWITHARROW_H
#include <QGraphicsItem>
#include <QPainter>
#define M_PI 3.14159265358979323846

class LineWithArrow : public QGraphicsItem
{
public:
    LineWithArrow(qreal x1, qreal y1, qreal x2, qreal y2);
    LineWithArrow(QPointF p1, QPointF p2);

protected:
    QRectF boundingRect() const;
    void   paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    qreal m_x1{0};
    qreal m_y1{0};
    qreal m_x2{100};
    qreal m_y2{100};
};

#endif  // LINEWITHARROW_H
