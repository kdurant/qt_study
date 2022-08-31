#ifndef LASERCONTROLLER_H
#define LASERCONTROLLER_H
#include <QtCore>

class LaserController : public QObject
{
    Q_OBJECT

public:
    // 不同类型的激光器使用的字段不一样
    struct LaserInfo
    {
        qint8  status;  // on or off
        qint32 work_time;

        qint32 freq_outside;
        qint32 freq_inside;

        qint32 expected_current;
        qint32 real_current;

        double temp;              //
        qint8  headTemp;          // 激光头温度
        qint32 ldTemp;            // 0-3000000(正值),3000000-6000000(负值)  温度值*10000,精确到小数点后第四位
        qint32 laserCrystalTemp;  // 激光晶体温度
        qint32 multiCrystalTemp;  // 倍频晶体温度

        quint8 statusBit;
        quint8 errorBit;
    } info;

    bool       isRecvNewData;  // 是否收到数据
    QByteArray recvData;

public:
    enum OpenMode
    {
        IN_SIDE  = 0x00,
        OUT_SIDE = 0x01,
    };

    LaserController() = default;

    // API函数准备好控制数据帧
    virtual bool setMode(OpenMode mode)
    {
        return false;
    };

    // 部分激光器以功率的形式
    // 部分激光器以电流的形式
    virtual bool setPower(quint16 power)
    {
        return false;
    };

    virtual bool setCurrent(quint16 current)
    {
        return false;
    };

    // 外触发时的工作频率
    virtual bool setFreq(qint32 freq)
    {
        return false;
    };

    virtual bool open(void)
    {
        return false;
    };
    virtual bool close(void)
    {
        return false;
    };

    virtual bool getStatus(void)
    {
        return false;
    }

    virtual bool checkself(void)
    {
        return false;
    }

    /**
     * @brief
     *
     * @return
     * -3: 激光器不支持此功能
     * -2: 未实现此功能
     * -1: 激光器通信异常
     * 0: 激光器正常复位
     */
    virtual int reset(void)
    {
        return -3;
    }

public slots:
    virtual void setNewData(const QByteArray& data)
    {
    }

signals:
    void sendDataReady(qint32 command, qint32 data_len, QByteArray& data);  // 需要发送的数据已经准备好
    void responseDataReady(void);
    void laserInfoReady(LaserInfo& data);
};
#endif
