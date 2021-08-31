#ifndef PUSI_H
#define PUSI_H
#include <QtCore>
#include "MotorController.h"

class PusiCOntroller : public MontorController
{
public:
    PusiCOntroller() = default;

public:
    bool init(void) override;
    bool start(void) override;
    bool stop(void) override;
    bool run(quint16 speed) override;

    qint8 calcFieldCRC(QByteArray& data)
    {
        qint8 ret = 0;
        for(auto i : data)
            ret += i;

        return ret;
    }

    bool turnStepsByNum(qint8 cAddr, qint32 nSteps);

    bool setMoveDirect(qint8 cAddr, bool bPositive);

    //设定细分数 可掉电保存
    bool SetSteps(qint8 cAddr, qint32 nSteps);
    //保存所有参数
    bool saveAllParas(qint8 cAddr);
    //设定最高转速
    bool setMostTurnSpeed(qint8 cAddr, qint32 nSpeed);
    //写驱动器地址
    bool writeDriveAddr(qint8 cAddr, qint32 nAddr);
    //设定最大相电流 400-4000 可掉电保存
    bool setMaxElectric(qint8 cAddr, qint32 nElectric);
    //设定外部紧急停止使能0、1、2、3
    bool setOutStopEnable(qint8 cAddr, qint32 nStopEnable);
    //设置减速度系数0-5 可掉电保存
    bool setReduceCoeff(qint8 cAddr, qint32 nReduceCoeff);
    //设定自动电流衰减使能0/1 可掉电保存
    bool setAutoElectricReduceEnable(qint8 cAddr, bool bEnable);
    //设置脱机使能 0/1
    bool setOfflineEnable(qint8 cAddr, bool bEnable);
    //设置当前位置 0-0x7fffffff
    bool setCurrentPosition(qint8 cAddr, qint32 nCurrentPosition);
    //IO端口写值 0-0xff
    bool writeIO(qint8 cAddr, qint32 nAddr);
    //设置加速度系数 0-5 可掉电保存
    bool setAcceCoeff(qint8 cAddr, qint32 nAcceCoeff);
    //当前步进命令终止 0
    bool stopTurning(qint8 cAddr);
    //设置外部触发方式0-3 可掉电保存
    bool setOutTriggerMode(qint8 cAddr, qint32 nTriggerMode);
    //设定离线自动运行 0/1 可掉电保存
    bool setOfflineAutoRun(qint8 cAddr, bool bAutoRUn);
    //设置启动速度 65-3000 可掉电保存
    bool setStartTurnSpeed(qint8 cAddr, qint32 nStartSpeed);
    //设置电流补偿因子 100-1200 可掉电保存
    bool setElectricCompensatoryFactor(qint8 cAddr, qint32 nElectricCompensatoryFactor);
    //设置速度模式使能 0/1
    bool setSpeedMode(qint8 cAddr, bool bSpeedMode);
    //设置速度补偿因子 300-2000 可掉电保存
    bool setSpeedCompensatoryFactor(qint8 cAddr, qint32 nSpeedCompensatoryFactor);
    //自动电流衰减系数 1-4 可掉电保存
    bool setAutoElectricReduceCoeff(qint8 cAddr, qint32 nReduceCoeff);
    //读写堵转长度0-0x3f 大于3f为读
    bool writeOrReadDuzhuanLen(qint8 cAddr, qint32 nDuzhuanLen);
    //设置停止速度 65-3000 可掉电保存
    bool setStopSpeed(qint8 cAddr, qint32 nStopSpeed);

    bool clearDuzhuanState(qint8 cAddr);

    //读写堵转配置寄存器0-0x7 大于7为读  写操作时：bit0:堵转检测使能  bit1：堵转后停止马达 bit2：堵转后GP05拉高
    bool writeOrReadDuzhuanRegister(qint8 cAddr, qint32 nDuzhuanRegister);
    //读写堵转触发值 0-0x1ff 大于0x1ff为读
    bool writeOrReadDuzhuanTrigger(qint8 cAddr, qint32 nDuzhuanTrigger);

    //读取当前位置?
    bool readCurrentPosition(qint8 cAddr);
    //读加速度系数
    bool readAcceCoeff(qint8 cAddr);
    //读取相电流设置
    bool readElectricValue(qint8 cAddr);
    //读取细分数设置
    bool readStepsValue(qint8 cAddr);
    //读取速度设置
    bool readSpeedValue(qint8 cAddr);
    //读控制器状态1 Bit[3:0]
    bool readControl1(qint8 cAddr);
    //IO端口读取 0-0x3ff
    bool readIO(qint8 cAddr, qint32 nAddr);
    //读取外部触发方式
    bool readOutTriggerMode(qint8 cAddr);
    //读取固件版本信息 0
    bool readHardwareVersion(qint8 cAddr);
    //读取控制器状态2
    bool readControl2(qint8 cAddr);
    //读取堵转位置
    QByteArray ReadDuzhuanPosition(qint8 cAddr);
    //读取减速度系数
    QByteArray ReadReduceCoeff(qint8 cAddr);
    //读取自动衰减因子
    QByteArray ReadAutoElectricReduceCoeff(qint8 cAddr);
    /**
     * @brief 清除 ext_stop1 标志位
     * @param addr
     */
    void clearExit1MarkBit(quint8 addr);
    void clearExit2MarkBit(quint8 addr);

private:
    bool       isRecvNewData{false};  // 是否收到数据
    QByteArray recvData;
    qint32     waitTime{1000};

    void waitResponse(quint16 waitMS)
    {
        QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
        connect(this, &PusiCOntroller::responseDataReady, &waitLoop, &QEventLoop::quit);
        QTimer::singleShot(waitMS, &waitLoop, &QEventLoop::quit);
        waitLoop.exec();
    }

public slots:

    void setNewData(QByteArray& data)
    {
        isRecvNewData = true;
        recvData      = data;
        emit responseDataReady();
    }

public:
    quint16 calcFieldCRC(quint16* pDataArray, quint16 numberofWords);  //CRC-CCITT
};

#endif
