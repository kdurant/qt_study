#ifndef PUSI_H
#define PUSI_H
#include <QtCore>
#include "MotorController.h"

class PusiController : public MontorController
{
public:
    PusiController() = default;

    enum MOVE_DIRECT
    {
        POSITIVE = 0x00000000,
        NEGTIVE  = 0x00000001,
    };
    enum WORK_STATUS
    {
        ENABLE  = 0x00000001,
        DISABLE = 0x00000000,
    };

    struct Status_Reg1
    {
        qint8 bit0_status;    // 0：IDLE；1：Busy
        qint8 bit1_ext1;      // 0：无急停事件；1：有急停事件
        qint8 bit2_ext2;      // 0：无急停事件；1：有急停事件
        qint8 bit3_auto_dec;  // 0：自动衰减不使能；1：自动衰减使能
        qint8 bit4_stall;     // 0：没有堵转；1：发生堵转
    };

    struct Status_Reg2
    {
        qint8 bit0_free_run;      // 0：脱机不使能；1：脱机使能
        qint8 bit1_ext1_en;       // 0：不使能急停；1：使能急停
        qint8 bit2_ext2_en;       // 0：不使能急停；1：使能急停
        qint8 bit3_speed_mode;    // 0：位移模式；1：速度模式
        qint8 bit4_dir;           // 0：反向；1：正向
        qint8 bit5_offline_auto;  // 0：离线不自动运行；1：离线自动运行
    };

    enum INSTRUCTION_SET
    {
        TURN_STEPS                        = 0x73,  // 转动给定的步数
        SET_SUB_DIVISION                  = 0x6D,  //设定细分数 默认32 0、2、4、8、16、32、64、128 可掉电保存
        CLEAR_BLOCK_STATUS                = 0x70,  //清堵转状态
        SAVE_ALL_PARA                     = 0x74,  //保存所有参数
        READ_CURRENT_POSITION             = 0x63,  //读取当前位置
        SET_MOVE_DIRECT                   = 0x64,  //设定转动方向
        SET_MAX_SPEED                     = 0x76,  //设定最高转速 1~16000pps(步/秒)
        WRITE_DRIVE_ADDR                  = 0x77,  //写驱动器地址 1~120
        READ_DECELE_COEFF                 = 0x68,  //读取减速度系数
        READ_ACCE_COEFF                   = 0x72,  //读加速度系数
        SET_MAX_ELECTRIC                  = 0x65,  //设定最大相电流400-4000mA 可掉电保存
        SET_OUT_STOP_ENABLE               = 0x66,  //设定外部紧急停止使能0、1、2、3
        READ_ELECTRIC_VALUE               = 0x6E,  //读取相电流设置
        READ_SUB_DIVISION                 = 0x6B,  //读取细分数设置
        READ_SPEED_SETTING                = 0x71,  //读取速度设置
        SET_REDUCE_COEFF                  = 0x6F,  //设置减速度系数0-5 可掉电保存
        SET_AUTO_ELECTRIC_REDUCE_ENABLE   = 0x61,  // 设定自动电流衰减使能0/1 可掉电保存
        SET_OFFLINE_ENABLE                = 0x67,  //设置脱机使能 0/1
        SET_CURRENT_POSITION              = 0x69,  // 设置当前位置 0-0x7fffffff
        READ_CONTROL_STATUS1              = 0x6A,  //读控制器状态1 Bit[3:0]
        CLEAR_EXT_STOP2_FLAG              = 0x6c,  //清除 ext_stop2标志位
        CLEAR_EXT_STOP1_FLAG              = 0x62,  //清除 ext_stop1标志位
        READ_IO_VALUE                     = 0x78,  //IO端口读取 0-0x3ff
        WRITE_IO_VALUE                    = 0x79,  //IO端口写值 0-0xff
        SET_ACCE_COEFF                    = 0x75,  //设置加速度系数 0-5 可掉电保存
        STOP_CURRENT_TURN                 = 0x49,  //当前步进命令终止 0
        SET_OUT_TRIGGER_MODE              = 0x4A,  //设置外部触发方式0-3 可掉电保存
        GET_OUT_TRIGGER_MODE              = 0x4B,  //读取外部触发方式
        SET_OFFLINE_AUTO_RUN              = 0x46,  //设定离线自动运行 0/1
        READ_HARDWARE_VERSION             = 0x52,  //读取固件版本信息 0
        SET_START_SPEED                   = 0x4C,  //设置启动速度 65-3000 可掉电保存
        SET_ELECTRIC_COMPENSATORY_FACTOR  = 0x4D,  //设置电流补偿因子 100-1200 可掉电保存
        SET_SPEED_MODE_ENABLE             = 0x4E,  //设置速度模式使能 0/1
        READ_CONTROL_STATUS2              = 0x4F,  //读取控制器状态2
        SET_SPEED_COMPENSATORY_FACTOR     = 0x50,  //设置速度补偿因子 300-2000 可掉电保存
        SET_AUTO_ELECTRIC_REDUCE_COEFF    = 0x51,  //自动电流衰减系数  1-4 可掉电保存
        WRITE_OR_READ_BLOCK_LENGTH        = 0x54,  //读写堵转长度0-0x3f 大于3f为读
        SET_STOP_SPEED                    = 0x53,  //设置停止速度 65-3000 可掉电保存
        READ_BLOCK_POSITION               = 0x58,  //读取堵转位置
        WRITE_OR_READ_BLOCK_REGISTER      = 0x59,  //读写堵转配置寄存器0-0x7 大于7为读  写操作时：bit0:堵转检测使能  bit1：堵转后停止马达 bit2：堵转后GP05拉高
        WRITE_OR_READ_BLOCK_TRIGGER_VALUE = 0x5A,  //读写堵转触发值 0-0x1ff 大于0x1ff为读

        READ_AUTO_REDUCE_COEFF = 0x55,  //读取自动衰减因子
    };

public:
    bool   init(void) override;
    bool   run(quint16 speed) override;
    bool   moveToPosition(quint32 postion) override;
    bool   moveToHome(void) override;
    bool   moveFixSpeed(quint32 speed) override;
    qint32 getActualVelocity(void) override;
    qint32 getActualPosition(void) override;

public:
    bool   turnStepsByNum(qint32 nSteps);
    bool   setSteps(qint32 nSteps);
    bool   clearBlockStatus(void);
    bool   saveAllParas();
    qint32 readCurrentPosition(void);
    bool   setMoveDirect(MOVE_DIRECT direct);
    bool   setMaxTurnSpeed(qint32 nSpeed);
    bool   ReadReduceCoeff(void);
    bool   readAcceCoeff(qint8 cAddr);
    bool   setMaxElectric(qint32 nElectric);
    bool   setOutStopEnable(qint32 nStopEnable);
    qint32 readElectricValue(void);
    qint32 readSubDiviSion(void);
    qint32 readSpeedSetting(void);
    bool   setReduceCoeff(qint32 coeff);
    bool   setAutoElectricReduceEnable(WORK_STATUS status);
    bool   setOfflineEnable(WORK_STATUS status);
    bool   setCurrentPosition(qint32 nCurrentPosition);
    qint32 readControl1(Status_Reg1& ret);
    void   clearExit2MarkBit(void);
    void   clearExit1MarkBit(void);
    bool   readIO(qint32 nAddr);
    bool   writeIO(qint32 nAddr);
    bool   writeDriveAddr(qint8 cAddr, qint32 nAddr);
    bool   setAcceCoeff(qint32 nAcceCoeff);
    bool   stopTurning(void);
    bool   setOutTriggerMode(qint32 nTriggerMode);
    qint32 readOutTriggerMode(void);
    bool   setOfflineAutoRun(WORK_STATUS status);
    qint32 readHardwareVersion(void);
    bool   setStartTurnSpeed(qint32 nStartSpeed);
    bool   setElectricCompensatoryFactor(qint32 nElectricCompensatoryFactor);
    bool   setSpeedMode(WORK_STATUS status);
    qint32 readControl2(void);
    bool   setSpeedCompensatoryFactor(qint32 data);
    bool   setAutoElectricReduceCoeff(qint32 nReduceCoeff);
    bool   setReduceCoeff(qint8 cAddr, qint32 nReduceCoeff);
    qint32 readBlockLen(void);
    bool   writeBlockLen(qint32 len);
    bool   setStopSpeed(qint32 nStopSpeed);
    qint32 readBlockPosition(void);
    qint32 readBlockRegister(void);
    bool   writeBlockRegister(qint32 reg);

    qint32 readBlockTriggerValue(void);
    bool   writeBlockTriggerValue(qint32 value);
    qint32 ReadAutoElectricReduceCoeff(void);

private:
    quint8 deviceAddr{0xff};
    bool   isInit{false};

    bool       isRecvNewData{false};  // 是否收到数据
    int        delayMs{500};
    QByteArray recvData;
    qint32     waitTime{1000};

    void waitResponse(quint16 waitMS)
    {
        QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
        connect(this, &PusiController::responseDataReady, &waitLoop, &QEventLoop::quit);
        QTimer::singleShot(waitMS, &waitLoop, &QEventLoop::quit);
        waitLoop.exec();
    }

    quint8 calcFieldCRC(QByteArray& data)
    {
        quint8 ret = 0;
        for(auto i : data)
            ret += i;

        return ret;
    }

    QByteArray encode(quint8 addr, quint8 cmd, quint32 data)
    {
        QByteArray frame;
        frame.append(0xA5);
        frame.append(addr);
        frame.append(cmd);
        // 发送整型数据时，低字节在前
        // 接收整型数据时，低字节在前
        frame.append(data);
        frame.append(data >> 8);
        frame.append(data >> 16);
        frame.append(data >> 24);
        frame.append(calcFieldCRC(frame));
        return frame;
    }
    quint8 getCommand(QByteArray& data) const
    {
        return data[2];
    }

    quint32 getData(QByteArray& data) const
    {
        quint32 ret;
        ret = ((static_cast<uint8_t>(data[6])) << 24) +
              ((static_cast<uint8_t>(data[5])) << 16) +
              ((static_cast<uint8_t>(data[4])) << 8) +
              ((static_cast<uint8_t>(data[3])) << 0);
        return ret;
    }

    bool compareCheckSum(QByteArray& data) const
    {
        quint8 r1 = 0;
        for(int i = 0; i < data.size() - 1; ++i)
            r1 += data[i];
        quint8 r2 = data.at(data.length() - 1);
        return r1 == r2;
    }

public slots:

    void setNewData(QByteArray& data) override
    {
        isRecvNewData = true;
        recvData      = data;
        emit responseDataReady();
    }

public:
    quint16 calcFieldCRC(quint16* pDataArray, quint16 numberofWords);  //CRC-CCITT
private:
    Status_Reg1 reg1{0, 0, 0, 0, 0};
    Status_Reg2 reg2{0, 0, 0, 0, 0, 0};
    qint32      position;  // 电机当前的位置，归零后清零
};

#endif
