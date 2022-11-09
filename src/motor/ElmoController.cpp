#include "ElmoController.h"

MotorController::MOTOR_STATUS Elmo::init()
{
    return MOTOR_STATUS::SUCCESS;
}

MotorController::MOTOR_STATUS Elmo::run(quint16 speed)
{
    moveFixSpeed(speed);
    return MOTOR_STATUS::SUCCESS;
}

qint32 Elmo::getActualVelocity()
{
    QByteArray frame = "VU\r";
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);

    if(recvData.contains("VU\r"))
    {
        int len = recvData.size();
        len -= 4;
        return recvData.mid(3, len).toHex().toUInt();
    }
    return 0;
}

qint32 Elmo::getActualPosition()
{
    QByteArray frame = "PU\r";
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);

    waitResponse(waitTime);
    if(recvData.contains("PU\r"))
    {
        int len = recvData.size();
        len -= 4;
        return recvData.mid(3, len).toHex().toUInt();
    }
    return 0;
}

bool Elmo::moveToHome()
{
    QVector<QByteArray> flow = {
        //        "UM=5\r",
        //        "MO=1\r",
        //        "OV[36]=100000\r",
        //        "OV[37]=100000\r",
        //        "OV[39]=0\r",
        //        "OV[38]=100000\r",
        //        "OV[35]=-4\r",
        //        "OF[7]=6\r",
        //        "CW=15\r",
        //        "CW=31\r",
        "MO=1\r",
        "OV[35]=-4\r",
        "OV[36]=100000\r",
        "OV[37]=50000\r",
        "OF[7]=6\r",
        "OF[7]\r",
        "CW=15\r",
        "CW=31\r",
    };

    for(auto &frame : flow)
    {
        emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
        waitResponse(waitTime);
    }

    return true;
}

// position unit: degree
// 1.4degree = 921
// 1degree = 658
MotorController::MOTOR_STATUS Elmo::moveToPosition(double postion, int direct)
{
    quint32 data = postion * 657.857;
    QString s    = "PA=" + QString::number(data, 10) + '\r';

    QVector<QByteArray> flow = {
        "FS=0\r",
        "MO=1\r",
        "PA=921\r",
        "MR[1]=0\r",
        "OV[36]=100000\r",
        "OV[37]=50000\r",
        "BG\r",
    };
    flow[2] = s.toUtf8();

    for(auto &frame : flow)
    {
        emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
        waitResponse(waitTime);
    }

    return MOTOR_STATUS::SUCCESS;
}

/**
 * @brief
 * 某相参数没有修改时，上位机是不会发送对应命令的
 * 10RPM 下发数据 39467
 * 20RPM 下发数据 78933
 * 30RPM 下发数据 118400
 * @param speed
 * @return
 */
bool Elmo::moveFixSpeed(quint32 speed)
{
    quint32 data = speed * 118430 / 30;
    QString sp   = "SP=" + QString::number(data, 10) + '\r';
    QString jv   = "JV=" + QString::number(data, 10) + '\r';

    QVector<QByteArray> flow = {
        "MO\r",
        "MO=1\r",
        "speed\r",
        "MO=1\r",
        "JogVelocity\r",
        "BG\r",
    };
    flow[2] = sp.toUtf8();
    flow[4] = jv.toUtf8();

    for(auto &frame : flow)
    {
        emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
        waitResponse(waitTime);
    }

    return true;
}
