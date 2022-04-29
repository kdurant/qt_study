#include "LineWithArrow.h"

#include <math.h>

LineWithArrow::LineWithArrow(qreal x1, qreal y1, qreal x2, qreal y2) :
    m_x1(x1),
    m_y1(y1),
    m_x2(x2),
    m_y2(y2)
{
}
LineWithArrow::LineWithArrow(QPointF p1, QPointF p2) :
    m_x1(p1.x()),
    m_y1(p1.y()),
    m_x2(p2.x()),
    m_y2(p2.y())
{
}

QRectF LineWithArrow::boundingRect() const
{
    return QRectF(0, 0, 100, 100);
}

void LineWithArrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);  //标明该参数没有使用
    Q_UNUSED(widget);
    painter->setRenderHint(QPainter::Antialiasing, true);
    QPen pen;
    pen.setColor(Qt::red);
    pen.setWidth(3);
    painter->setPen(pen);

    QLineF line = QLineF(m_x1, m_y1, m_x2, m_y2);

    qreal arrowSize = 20;

    double  angle   = std::atan2(-line.dy(), line.dx());
    QPointF arrowP1 = line.p2() - QPointF(sin(angle + M_PI / 3) * arrowSize,
                                          cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = line.p2() - QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                          cos(angle + M_PI - M_PI / 3) * arrowSize);

    QPolygonF arrowHead;
    arrowHead.clear();
    arrowHead << line.p2() << arrowP1 << arrowP2;
    painter->drawLine(line);
    painter->drawPolygon(arrowHead);
}
