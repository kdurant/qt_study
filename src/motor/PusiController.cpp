#include "PusiController.h"

bool PusiController::init()
{
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
    return false;
}

bool PusiController::moveFixSpeed(quint32 speed)
{
    return false;
}

qint32 PusiController::getActualVelocity()
{
    return false;
}

qint32 PusiController::getActualPosition()
{
    return false;
}

bool PusiController::turnStepsByNum(qint32 nSteps)
{
    if(nSteps < 1)
        nSteps = 1;
    if(nSteps > 0x7fffffff)
        nSteps = 0x7fffffff;
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::TURN_STEPS, nSteps);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);

    return true;
}

bool PusiController::setSteps(qint32 nSteps)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_SUB_DIVISION, nSteps);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    return true;
}

bool PusiController::clearBlockStatus(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::CLEAR_BLOCK_STATUS, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    return true;
}

bool PusiController::saveAllParas()
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SAVE_ALL_PARA, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    return true;
}

bool PusiController::readCurrentPosition(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_CURRENT_POSITION, 0);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);

    waitResponse(delayMs);
    if(isRecvNewData)
    {
        isRecvNewData = false;
        if(compareCheckSum(recvData))
        {
            if(getCommand(recvData) == INSTRUCTION_SET::READ_CURRENT_POSITION)
                return getData(recvData);
        }
    }
    return true;
}

bool PusiController::setMoveDirect(MOVE_DIRECT direct)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_MOVE_DIRECT, direct);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
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
    return true;
}

bool PusiController::ReadReduceCoeff(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_DECELE_COEFF, 0);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    return true;
}

bool PusiController::readAcceCoeff(qint8 cAddr)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_ACCE_COEFF, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
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
    return true;
}

bool PusiController::setOutStopEnable(qint32 nStopEnable)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_OUT_STOP_ENABLE, nStopEnable);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    return true;
}

qint32 PusiController::readElectricValue(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_ELECTRIC_VALUE, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
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
            if(getCommand(recvData) == INSTRUCTION_SET::READ_SUB_DIVISION)
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
            if(getCommand(recvData) == INSTRUCTION_SET::READ_SPEED_SETTING)
                return getData(recvData);
        }
    }

    return -1;
}

bool PusiController::setReduceCoeff(qint32 coeff)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_REDUCE_COEFF, coeff);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    return true;
}

bool PusiController::setAutoElectricReduceEnable(WORK_STATUS status)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_AUTO_ELECTRIC_REDUCE_ENABLE, status);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    return true;
}

bool PusiController::setOfflineEnable(WORK_STATUS status)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_OFFLINE_ENABLE, status);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
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
    return true;
}

qint32 PusiController::readControl1(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_CONTROL_STATUS1, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);

    waitResponse(delayMs);
    if(isRecvNewData)
    {
        isRecvNewData = false;
        if(compareCheckSum(recvData))
        {
            if(getCommand(recvData) == INSTRUCTION_SET::READ_CONTROL_STATUS1)
                return getData(recvData);
        }
    }

    return -1;
}

void PusiController::clearExit2MarkBit(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::CLEAR_EXT_STOP2_FLAG, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
}

void PusiController::clearExit1MarkBit(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::CLEAR_EXT_STOP1_FLAG, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
}

bool PusiController::readIO(qint32 nAddr)
{
    if(nAddr < 0)
        nAddr = 0;
    if(nAddr > 0x3ff)
        nAddr = 0x3ff;

    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_IO_VALUE, nAddr);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
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
    return true;
}

bool PusiController::stopTurning(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::STOP_CURRENT_TURN, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
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
    return true;
}

qint32 PusiController::readOutTriggerMode(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::GET_OUT_TRIGGER_MODE, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    return 0;
}

bool PusiController::setOfflineAutoRun(WORK_STATUS status)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_OFFLINE_AUTO_RUN, WORK_STATUS::ENABLE);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    return true;
}

qint32 PusiController::readHardwareVersion(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_HARDWARE_VERSION, 0);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
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
    return true;
}

bool PusiController::setSpeedMode(WORK_STATUS status)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::SET_SPEED_MODE_ENABLE, status);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    return true;
}

qint32 PusiController::readControl2(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_CONTROL_STATUS2, 0);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
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
    return true;
}
bool PusiController::writeOrReadDuzhuanLen(qint32 nDuzhuanLen)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::WRITE_OR_READ_BLOCK_LENGTH, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    return true;
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
    return true;
}

qint32 PusiController::readBlockPosition(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_BLOCK_POSITION, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    return true;
}

bool PusiController::writeOrReadBlockRegister(qint32 data)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::WRITE_OR_READ_BLOCK_REGISTER, data);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    return true;
}

bool PusiController::writeOrReadDuzhuanTrigger(qint32 data)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::WRITE_OR_READ_BLOCK_TRIGGER_VALUE, data);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    return true;
}

qint32 PusiController::ReadAutoElectricReduceCoeff(void)
{
    QByteArray frame = encode(deviceAddr, INSTRUCTION_SET::READ_AUTO_REDUCE_COEFF, 0);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
    return true;
}
