/*
 * 1. 具有两个限位开关，分别对应着EXT1, 和EXT2
 * 2. 当电机运动到限位开关，触发EXT1或EXT2，此时断电。
 *      再重启后，还往限位开关所在的方向运动，会导致电机抖动，且不触发EXT1或EXT2信号
 * 3. 运动方向设置为方向时，对应限位开关为EXT1
 * 4. 上电读取电机位置为0, 不同的运动方向会导致数据溢出
 */
#include "PusiController.h"

bool PusiController::init()
{
    writeBlockTriggerValue(0xff);
    writeBlockLen(0x2f);
    writeBlockRegister(0x01);
    readControl1(reg1);

    return false;
}

bool PusiController::run(quint16 speed)
{
    return false;
}

bool PusiController::moveToPosition(quint32 postion)
{
    return false;
}

bool PusiController::moveToHome()
{
    bool status;
    int  postion;

    writeBlockTriggerValue(0xff);
    writeBlockLen(0x2f);
    writeBlockRegister(0x01);

    clearExit1MarkBit();
    clearExit2MarkBit();
    setMaxTurnSpeed(2000);
    postion = readCurrentPosition();

    qDebug() << "step1: 反转，找EXT1";
    setMoveDirect(PusiController::NEGTIVE);  // 找EXT1
    turnStepsByNum(10000);
    readControl1(reg1);
    if(reg1.bit1_ext1 == 1)
    {
        qDebug() << "step1-a: 找到EXT1";
        clearExit1MarkBit();
        goto start_pos;
    }

    qDebug() << "step2: 正转，找EXT2";
    setMoveDirect(PusiController::POSITIVE);  // 找EXT2
    turnStepsByNum(10000);
    readControl1(reg1);
    if(reg1.bit2_ext2 == 1)
    {
        qDebug() << "step2-a: 找到EXT2";
        clearExit2MarkBit();
    }

    qDebug() << "step3: 一直反转，直到找到EXT1";
    setMoveDirect(PusiController::NEGTIVE);
    while(reg1.bit1_ext1 == 0)
    {
        status = turnStepsByNum(50000);  // 刚好是从EXT2到EXT1需要的步数
        readControl1(reg1);
    }

    while(reg1.bit1_ext1 == 1 || reg1.bit2_ext2 == 1)
    {
        clearExit1MarkBit();
        clearExit2MarkBit();
        qDebug() << "step3-a: 清除EXT1，EXT2";
    }
    readControl1(reg1);

start_pos:
    qDebug() << "step4: 正转1864step, 归零完成";
    setMoveDirect(PusiController::POSITIVE);
    turnStepsByNum(1864);
    setCurrentPosition(0);
    return true;
}

bool PusiController::moveFixSpeed(quint32 speed)
{
    return false;
}

qint32 PusiController::getActualVelocity()
{
    return 0;
}

qint32 PusiController::getActualPosition()
{
    return static_cast<int>(readCurrentPosition());
}

/**
* @brief 控制电机运动指定的步数
* @param nSteps
* @return 如果电机实际运动的步数过小，说明碰到了限位开关
*         如果限位开关标志被置1，正常
*         如果限位开关标志没有被置1，说明上次刚好停在限位开关位置
*/
bool PusiController::turnStepsByNum(qint32 nSteps)
{
    if(nSteps < 1)
        nSteps = 1;
    if(nSteps > 0x7fffffff)
        nSteps = 0x7fffffff;

    qDebug() << "           ------------运动起始位置：" << readCurrentPosition();

    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::TURN_STEPS, nSteps);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);

    readControl1(reg1);
    while(reg1.bit0_status != 0)  // 等待电机运动结束
    {
        Common::sleepWithoutBlock(500);
        readControl1(reg1);
    }
    qDebug() << "           ############运动停止位置：" << readCurrentPosition();

    return true;
}

bool PusiController::setSteps(qint32 nSteps)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_SUB_DIVISION, nSteps);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::clearBlockStatus(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::CLEAR_BLOCK_STATUS, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::saveAllParas()
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SAVE_ALL_PARA, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

qint32 PusiController::readCurrentPosition(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_CURRENT_POSITION, 0);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);

    waitResponse(delayMs);
    if(isRecvNewData)
    {
        isRecvNewData = false;
        if(compareCheckSum(recvData))
        {
            if(getCommand(recvData) == 0xff)
                return getData(recvData);
        }
    }
    return -1;
}

bool PusiController::setMoveDirect(MOVE_DIRECT direct)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_MOVE_DIRECT, direct);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::setMaxTurnSpeed(qint32 nSpeed)
{
    if(nSpeed < 1)
        nSpeed = 1;
    if(nSpeed > 16000)
        nSpeed = 16000;
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_MAX_SPEED, nSpeed);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::ReadReduceCoeff(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_DECELE_COEFF, 0);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::readAcceCoeff(qint8 cAddr)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_ACCE_COEFF, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::setMaxElectric(qint32 nElectric)
{
    if(nElectric < 200)
        nElectric = 200;
    if(nElectric > 4000)
        nElectric = 4000;

    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_MAX_ELECTRIC, nElectric);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::setOutStopEnable(qint32 nStopEnable)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_OUT_STOP_ENABLE, nStopEnable);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

qint32 PusiController::readElectricValue(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_ELECTRIC_VALUE, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return 0;
}

qint32 PusiController::readSubDiviSion(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_SUB_DIVISION, 0);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);

    waitResponse(delayMs);
    if(isRecvNewData)
    {
        isRecvNewData = false;
        if(compareCheckSum(recvData))
        {
            if(getCommand(recvData) == 0xff)
                return getData(recvData);
        }
    }

    return -1;
}

// 0x71

qint32 PusiController::readSpeedSetting(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_SPEED_SETTING, 0);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);

    waitResponse(delayMs);
    if(isRecvNewData)
    {
        isRecvNewData = false;
        if(compareCheckSum(recvData))
        {
            if(getCommand(recvData) == 0xff)
                return getData(recvData);
        }
    }

    return -1;
}

bool PusiController::setReduceCoeff(qint32 coeff)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_REDUCE_COEFF, coeff);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::setAutoElectricReduceEnable(WORK_STATUS status)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_AUTO_ELECTRIC_REDUCE_ENABLE, status);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::setOfflineEnable(WORK_STATUS status)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_OFFLINE_ENABLE, status);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::setCurrentPosition(qint32 nCurrentPosition)
{
    if(nCurrentPosition < 0)
        nCurrentPosition = 0;
    if(nCurrentPosition > 0x7fffffff)
        nCurrentPosition = 0x7fffffff;

    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_CURRENT_POSITION, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

qint32 PusiController::readControl1(Status_Reg1& ret)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_CONTROL_STATUS1, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);

    waitResponse(delayMs);
    if(isRecvNewData)
    {
        isRecvNewData = false;
        if(compareCheckSum(recvData))
        {
            if(getCommand(recvData) == 0xff)
            {
                quint32 data      = getData(recvData);
                ret.bit0_status   = data & 0x01;
                ret.bit1_ext1     = (data >> 1) & 0x01;
                ret.bit2_ext2     = (data >> 2) & 0x01;
                ret.bit3_auto_dec = (data >> 3) & 0x01;
                ret.bit4_stall    = (data >> 4) & 0x01;
                return 0;
            }
        }
    }

    return -1;
}

void PusiController::clearExit2MarkBit(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::CLEAR_EXT_STOP2_FLAG, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
}

void PusiController::clearExit1MarkBit(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::CLEAR_EXT_STOP1_FLAG, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
}

bool PusiController::readIO(qint32 nAddr)
{
    if(nAddr < 0)
        nAddr = 0;
    if(nAddr > 0x3ff)
        nAddr = 0x3ff;

    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_IO_VALUE, nAddr);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::writeIO(qint32 nAddr)
{
    if(nAddr < 0)
        nAddr = 0;
    if(nAddr > 0xff)
        nAddr = 0xff;
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::WRITE_IO_VALUE, nAddr);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::writeDriveAddr(qint8 cAddr, qint32 nAddr)
{
    if(nAddr < 1)
        nAddr = 1;
    if(nAddr > 120)
        nAddr = 120;
    QByteArray frame;
    frame.append(0xA5);
    frame.append(cAddr);
    frame.append(0x77);
    frame.append(nAddr);
    frame.append(nAddr >> 8);
    frame.append(nAddr >> 16);
    frame.append(nAddr >> 24);
    frame.append(calcFieldCRC(frame));

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::setAcceCoeff(qint32 nAcceCoeff)
{
    if(nAcceCoeff < 0)
        nAcceCoeff = 0;
    if(nAcceCoeff > 5)
        nAcceCoeff = 5;

    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_ACCE_COEFF, nAcceCoeff);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::stopTurning(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::STOP_CURRENT_TURN, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::setOutTriggerMode(qint32 nTriggerMode)
{
    if(nTriggerMode < 0)
        nTriggerMode = 0;
    if(nTriggerMode > 3)
        nTriggerMode = 3;

    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_OUT_TRIGGER_MODE, nTriggerMode);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

qint32 PusiController::readOutTriggerMode(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::GET_OUT_TRIGGER_MODE, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return 0;
}

bool PusiController::setOfflineAutoRun(WORK_STATUS status)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_OFFLINE_AUTO_RUN, WORK_STATUS::ENABLE);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

qint32 PusiController::readHardwareVersion(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_HARDWARE_VERSION, 0);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::setStartTurnSpeed(qint32 nStartSpeed)
{
    if(nStartSpeed != 0)
    {
        if(nStartSpeed < 65)
            nStartSpeed = 65;
        if(nStartSpeed > 3000)
            nStartSpeed = 3000;
    }

    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_START_SPEED, nStartSpeed);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::setElectricCompensatoryFactor(qint32 nElectricCompensatoryFactor)
{
    if(nElectricCompensatoryFactor != 0)
    {
        if(nElectricCompensatoryFactor < 100)
            nElectricCompensatoryFactor = 100;
        if(nElectricCompensatoryFactor > 1200)
            nElectricCompensatoryFactor = 1200;
    }
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_ELECTRIC_COMPENSATORY_FACTOR, nElectricCompensatoryFactor);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::setSpeedMode(WORK_STATUS status)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_SPEED_MODE_ENABLE, status);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

qint32 PusiController::readControl2(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_CONTROL_STATUS2, 0);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return 0;
}
bool PusiController::setSpeedCompensatoryFactor(qint32 data)
{
    if(data != 0)  //=0为读取
    {
        if(data < 300)
            data = 300;
        if(data > 2000)
            data = 2000;
    }
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_SPEED_COMPENSATORY_FACTOR, data);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

bool PusiController::setAutoElectricReduceCoeff(qint32 nReduceCoeff)
{
    if(nReduceCoeff < 1)
        nReduceCoeff = 1;
    if(nReduceCoeff > 4)
        nReduceCoeff = 4;

    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_AUTO_ELECTRIC_REDUCE_COEFF, nReduceCoeff);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

qint32 PusiController::readBlockLen(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::WRITE_OR_READ_BLOCK_LENGTH, 0x4f);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);

    if(isRecvNewData)
    {
        isRecvNewData = false;
        if(compareCheckSum(recvData))
        {
            if(getCommand(recvData) == 0xff)
                return getData(recvData);
        }
    }
    return -1;
    return true;
}

// 2. 使用0x54设置堵转触发值
bool PusiController::writeBlockLen(qint32 len)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::WRITE_OR_READ_BLOCK_LENGTH, len);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
}

bool PusiController::setStopSpeed(qint32 nStopSpeed)
{
    if(nStopSpeed != 0)
    {
        if(nStopSpeed < 65)
            nStopSpeed = 65;
        if(nStopSpeed > 3000)
            nStopSpeed = 3000;
    }
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_STOP_SPEED, nStopSpeed);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

// 4. 使用0x58命令读取指令位置
qint32 PusiController::readBlockPosition(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_BLOCK_POSITION, 1);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);

    if(isRecvNewData)
    {
        isRecvNewData = false;
        if(compareCheckSum(recvData))
        {
            if(getCommand(recvData) == 0xff)
                return getData(recvData);
        }
    }
    return -1;
}

qint32 PusiController::readBlockRegister()
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::WRITE_OR_READ_BLOCK_REGISTER, 0x0f);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    if(isRecvNewData)
    {
        isRecvNewData = false;
        if(compareCheckSum(recvData))
        {
            if(getCommand(recvData) == 0xff)
                return getData(recvData);
        }
    }
    return -1;
}

// 3. 使用0x59设置堵转寄存器
bool PusiController::writeBlockRegister(qint32 reg)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::WRITE_OR_READ_BLOCK_REGISTER, reg);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
}

qint32 PusiController::readBlockTriggerValue(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::WRITE_OR_READ_BLOCK_TRIGGER_VALUE, 1024);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);

    if(isRecvNewData)
    {
        isRecvNewData = false;
        if(compareCheckSum(recvData))
        {
            if(getCommand(recvData) == 0xff)
                return getData(recvData);
        }
    }
    return -1;
}

// 1. 使用0x5a设置堵转触发值
bool PusiController::writeBlockTriggerValue(qint32 value)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::WRITE_OR_READ_BLOCK_TRIGGER_VALUE, value);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}

qint32 PusiController::ReadAutoElectricReduceCoeff(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_AUTO_REDUCE_COEFF, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    waitResponse(delayMs);
    return true;
}
