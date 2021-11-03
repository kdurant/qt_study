#ifndef MOTORCONTROLL_H
#define MOTORCONTROLL_H
#include <QtCore>

#include "common.h"
#include "protocol.h"

class MontorController : public QObject
{
    Q_OBJECT
public:
    MontorController() = default;

    /**
    * @brief 初始化电机
    *
    * @return 
    */
    virtual bool init() = 0;

    /**
     * @brief 在电机已经运动的情况下，锈点电机运动速度(r/s)
     * @param speed
     * @return
     */
    virtual bool run(quint16 speed) = 0;

    virtual bool moveToPosition(double postion) = 0;

    virtual bool moveToHome(void) = 0;

    /**
    * @brief 初始化电机，并让电机以指定速度运动
    *
    * @param speed
    *
    * @return 
    */
    virtual bool moveFixSpeed(quint32 speed) = 0;

    /**
    * @brief 读取电机实际的运动速度
    *
    * @return 
    */
    virtual qint32 getActualVelocity(void) = 0;

    /**
    * @brief 读取电机实际的位置
    *
    * @return 
    */
    virtual qint32 getActualPosition(void) = 0;

public slots:
    virtual void setNewData(QByteArray& data) = 0;

signals:
    void sendDataReady(QByteArray& data);  // 需要发送的数据已经准备好

    void sendDataReady(qint32 command, qint32 data_len, QByteArray& data);  // 需要发送的数据已经准备好

    void responseDataReady(void);  // 接收到响应数据
};
#endif
