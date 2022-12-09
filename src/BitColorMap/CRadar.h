#ifndef CRADAR_H
#define CRADAR_H

#include <QtCore>

#include <QObject>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QWheelEvent>

class CRadar : public QGraphicsView
{
private:
    double RADIUS{200};
    double MAX_DISTANCE{2000};
    double pi{3.1415926};

    QPointF m_leftCenter{-RADIUS, 0};
    QPointF m_rightCenter{RADIUS, 0};
    QPointF m_topCenter{0, -RADIUS};
    QPointF m_bottomCenter{0, RADIUS};
    QPointF m_center{0, 0};

    QVector<double> m_angle;
    QVector<double> m_distance;

    QPointF m_lastPointF;
    bool    isPressed{false};
    qreal   factor{1};

    QGraphicsScene *scene{new QGraphicsScene};

public:
    CRadar()
    {
        //        setDragMode(QGraphicsView::RubberBandDrag);
        //        setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);
        //        setAcceptDrops(true);
        initUI();
    }

    void initUI(void)
    {
        QPen pen;
        pen.setColor(Qt::red);
        pen.setWidth(3);
        scene->addEllipse(QRectF(QPointF(-RADIUS, -RADIUS), QPointF(RADIUS, RADIUS)), pen);
        scene->addEllipse(QRectF(QPointF(-RADIUS / 2, -RADIUS / 2), QPointF(RADIUS / 2, RADIUS / 2)), pen);
        scene->addEllipse(QRectF(QPointF(-RADIUS / 4, -RADIUS / 4), QPointF(RADIUS / 4, RADIUS / 4)), pen);
        scene->addLine(QLineF(m_leftCenter, m_rightCenter), pen);
        scene->addLine(QLineF(m_topCenter, m_bottomCenter), pen);

        QGraphicsTextItem *str = new QGraphicsTextItem("90°");
        str->setDefaultTextColor(QColor(Qt::magenta));
        str->setPos(m_topCenter + QPoint(-10, -20));
        scene->addItem(str);

        str = new QGraphicsTextItem("270°");
        str->setDefaultTextColor(QColor(Qt::magenta));
        str->setPos(m_bottomCenter + QPoint(-10, 2));
        scene->addItem(str);

        str = new QGraphicsTextItem(QString::number(MAX_DISTANCE, 'g', 4) + "m");
        str->setDefaultTextColor(QColor(Qt::magenta));
        str->setPos(m_topCenter);
        scene->addItem(str);

        str = new QGraphicsTextItem(QString::number(MAX_DISTANCE / 2, 'g', 4) + "m");
        str->setDefaultTextColor(QColor(Qt::magenta));
        str->setPos(m_topCenter / 2);
        scene->addItem(str);

        str = new QGraphicsTextItem(QString::number(MAX_DISTANCE / 4, 'g', 4) + "m");
        str->setDefaultTextColor(QColor(Qt::magenta));
        str->setPos(m_topCenter / 4);
        scene->addItem(str);

        setScene(scene);
    }

    /**
     * @brief updateData
     * @param angle
     * @param distance
     * @param max_distance
     */
    void updateData(QVector<double> &angle, QVector<double> &distance, double max_distance)
    {
        m_angle    = angle;
        m_distance = distance;

        if(max_distance < 0)
        {
            auto max     = std::max_element(std::begin(distance), std::end(distance));
            MAX_DISTANCE = *max * 1.2;
        }

        scene->clear();
        initUI();

        QPen pen;
        pen.setColor(Qt::green);
        pen.setWidthF(1);
        QBrush brush;
        brush.setColor(Qt::green);

        double x      = 0;
        double y      = 0;
        int    offset = 0;
        for(int i = 0; i < angle.size(); i++)
        {
            offset = distance[i] * RADIUS / MAX_DISTANCE;
            x      = qCos((angle[i] * pi) / 180);
            y      = qSin((angle[i] * pi) / 180);
            x *= offset;
            y *= offset;
            x = -x;
            y = -y;
            scene->addEllipse(x, y, 1, 1, pen, brush);
        }
    }

protected:
    void wheelEvent(QWheelEvent *event)
    {
        double scaleFactor = pow(2., event->angleDelta().y() / 240.0);

        factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();

        if(factor < 0.07 || factor > 100)
            return;
        scale(scaleFactor, scaleFactor);
    }
    void mousePressEvent(QMouseEvent *mouseEvent)
    {
        m_lastPointF = mouseEvent->pos();
        isPressed    = true;

        QGraphicsView::mousePressEvent(mouseEvent);
    }

    void mouseMoveEvent(QMouseEvent *mouseEvent)
    {
        if(isPressed)
        {
            QPointF disPointF = mouseEvent->pos() - m_lastPointF;
            m_lastPointF      = mouseEvent->pos();
            this->scene->setSceneRect(this->scene->sceneRect().x() - disPointF.x(),
                                      this->scene->sceneRect().y() - disPointF.y(),
                                      this->scene->sceneRect().width(),
                                      this->scene->sceneRect().height());
            scene->update();
        }

        QGraphicsView::mouseMoveEvent(mouseEvent);
    }

    void mouseReleaseEvent(QMouseEvent *mouseEvent)
    {
        if(isPressed)
        {
            isPressed = false;
        }

        QGraphicsView::mouseReleaseEvent(mouseEvent);
    }
};
#endif
