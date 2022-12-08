#ifndef CRadar_H
#define CRadar_H
#include <QPainter>
#include <QtCore>
#include <QWidget>

class CRadar : public QWidget
{
private:
    int RADIUS{200};

    int m_width{400};
    QPoint  m_leftCenter{-RADIUS, 0};
    QPoint  m_rightCenter{RADIUS, 0};
    QPoint  m_topCenter{0, -RADIUS};
    QPoint  m_bottomCenter{0, RADIUS};
    QPoint  m_center{0, 0};

    QVector<double> m_angle;
    QVector<double> m_distance;

    int MAX_DISTANCE{2000};
    double pi{3.1415926};

public:
    CRadar()
    {
        resize(400, 400);

        m_angle.append(0);
        m_distance.append(111);
    }
    void updateData(QVector<double> &angle, QVector<double> &distance)
    {
        m_angle = angle;
        m_distance = distance;
    }

    void clearUI(void)
    {

    }

    void refreshUI(QPainter *painter)
    {
        painter->setPen(Qt::red);
        painter->setWindow(-(RADIUS+50), -(RADIUS+50), (RADIUS+50)*2, (RADIUS+50)*2);
        QRectF rect = QRectF(-RADIUS, -RADIUS, RADIUS*2, RADIUS*2);
                painter->drawEllipse(rect);

        painter->drawLine(m_leftCenter, m_rightCenter);
        painter->drawLine(m_bottomCenter, m_topCenter);
        painter->drawText(m_topCenter + QPoint(0, -15), "90 degree");
        painter->drawText(m_bottomCenter + QPoint(0, 15), "270 degree");

        painter->drawText(m_topCenter + QPoint(0, 15), "2000m");

        painter->drawEllipse(QRectF(QPointF(-RADIUS/2, -RADIUS/2) ,
                                   QPointF(RADIUS/2, RADIUS/2))
                            );

        painter->drawText(m_topCenter  + QPoint(0, RADIUS/2 + 15), "1000m");

        painter->setPen(QPen(Qt::green, 6, Qt::SolidLine, Qt::RoundCap));

        double x = 0;
        double y = 0;
        for(int i = 0; i < m_angle.size(); i++)
        {
            int offset = m_distance[i]* m_width/2/MAX_DISTANCE ;
            x = qCos((m_angle[i] * pi) / 180);
            y = qSin((m_angle[i] * pi) / 180);
            x *= offset;
            y *= offset;
            x =  - x;
            y =  - y;

            painter->drawPoint(QPoint(x, y));
        }
    }

protected:
    //    void resizeEvent(QResizeEvent *event);  //大小重置事件
    void paintEvent(QPaintEvent *event)
    {
        QPainter painter;
        painter.begin(this);
        painter.setRenderHint(QPainter::Antialiasing);
        refreshUI(&painter);
        painter.end();
    }
    void wheelEvent(QWheelEvent *event)
    {


    }


};

#endif
