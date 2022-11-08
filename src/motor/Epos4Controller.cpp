#include "Epos4Controller.h"

/**
 * @brief 设置电机默认工作参数
 * @return
 */
MotorController::MOTOR_STATUS EPOS4::init()
{
    // 电机重新上电后，这4步是必须的
    if(!clearFault())
        return MOTOR_STATUS::FAILED;
    setShutdown();
    setHalt();
    setProfileVelocityMode();

    // 这4步是可选的
    setMaximalProfileVelocity(3571);
    setQuickstopDeceleration(45);
    setProfileAcceleration(45);
    setProfileDeceleration(45);
    return MOTOR_STATUS::SUCCESS;
}

/**
 * @brief 需要在电机初始化后才能进行这个操作
 * @param speed
 * @return
 */
MotorController::MOTOR_STATUS EPOS4::run(quint16 speed)
{
    if(!setTargetVelocity(speed))
        return MOTOR_STATUS::FAILED;
    if(!setEnableState())
        return MOTOR_STATUS::FAILED;

    return MOTOR_STATUS::SUCCESS;
}

/**
 * @brief 移动电机到指定位置, 定位模式下使用
 * @param postion
 * @return
 */
MotorController::MOTOR_STATUS EPOS4::moveToPosition(double postion, int direct)
{
    if(!clearFault())
        return MOTOR_STATUS::FAILED;
    setProfilePositionMode();
    setProfileVelocity(500);
    setQuickstopDeceleration(45);
    setProfileAcceleration(45);
    setProfileDeceleration(45);
    setTargetPosition(static_cast<quint32>(postion));
    setAbsolutePositionStartImmdeitaly();

    return MOTOR_STATUS::SUCCESS;
}

/**
 * @brief 电机以固定的速度转动, 定速模式下使用
 * init()和run()的结合
 * @param speed
 * @return
 */
bool EPOS4::moveFixSpeed(quint32 speed)
{
    clearFault();
    setEnableState();
    setHalt();
    setProfilePositionMode();
    setMaximalProfileVelocity(3571);
    setQuickstopDeceleration(45);
    setProfileAcceleration(45);
    setProfileDeceleration(45);
    setTargetVelocity(speed);
    setEnableState();
    return true;
}

/**
 * @brief 摆扫模式
 * @param start_pos
 * @param end_pos
 * @return
 */
bool EPOS4::sweep(quint32 start_pos, quint32 end_pos)
{
    static bool running = false;
    if(start_pos > end_pos)
    {
        return false;
    }
    else if(start_pos == end_pos)  // 停止条件
    {
        moveToPosition(0, 1);
        running = false;
    }
    else
    {
        running   = true;
        start_pos = (start_pos / 360.0) * 163840;
        end_pos   = (end_pos / 360.0) * 163840;

        moveToPosition(0, 1);
    }
    qDebug() << "running value = " << running;

    quint32 current_pos = 0;
    quint32 range_min   = 0;
    quint32 range_max   = 0;
    while(running)
    {
        qDebug() << "running value(in while) = " << running;
        range_min = end_pos * 0.95;
        range_max = end_pos * 1.05;
        moveToPosition(end_pos, 1);

        current_pos = getActualPosition();
        while(current_pos < range_min)
        {
            current_pos = getActualPosition();
            QEventLoop waitLoop;
            QTimer::singleShot(5, &waitLoop, &QEventLoop::quit);
            waitLoop.exec();
        }

        range_min = start_pos * 0.95;
        range_max = start_pos * 1.05;
        moveToPosition(start_pos, 1);

        current_pos = getActualPosition();
        while(current_pos > range_max)
        {
            current_pos = getActualPosition();
            QEventLoop waitLoop;
            QTimer::singleShot(5, &waitLoop, &QEventLoop::quit);
            waitLoop.exec();
        }
    }
    return true;
}

bool EPOS4::moveToHome()
{
    if(!clearFault())
        return false;
    setHomeMode();
    setPositiveSpeed();
    setShutdown();
    setEnableState();
    setHalt();
    startHoming();
    return true;
}

quint16 EPOS4::calcFieldCRC(quint16 *pDataArray, quint16 numberofWords)
{
    quint16 shifter, c;
    quint16 carry;
    quint16 CRC = 0;

    // Calculate pDataArray Word by Word
    while(numberofWords--)
    {
        shifter = 0x8000;
        c       = *pDataArray++;
        do
        {
            carry = CRC & 0x8000;
            CRC <<= 1;
            if(c & shifter)
                CRC++;
            if(carry)
                CRC ^= 0x1021;
            shifter >>= 1;
        } while(shifter);
    }
    return CRC;
}

QVector<quint16> EPOS4::WordPlusCRC(QVector<quint16> word)
{
    word.append(0x0000);  // zeroWord

    quint16  numberOfWords  = word.length();
    quint16 *ptr            = &word[0];
    quint16  crc            = calcFieldCRC(ptr, numberOfWords);
    word[numberOfWords - 1] = crc;
    return word;
}

QByteArray EPOS4::transmitWord2Byte(QVector<quint16> word)
{
    QByteArray array;
    array.resize(word.length() * 2);
    array[0] = (quint8)((word[0] & 0xff00) >> 8);
    array[1] = (quint8)(word[0] & 0x00ff);
    for(int i = 1; i < word.length(); i++)
    {
        array[2 * i]     = (quint8)(word[i] & 0x00ff);
        array[2 * i + 1] = (quint8)((word[i] & 0xff00) >> 8);
    }
    return array;
}

bool EPOS4::setDisableState()
{
    QVector<quint16> word{0x9002, 0x6804, 0x6040, 0x0100, 0x0000, 0x0000};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

bool EPOS4::clearFault()
{
    QVector<quint16> word{0x9002, 0x6804, 0x6040, 0x0100, 0x0080, 0x0000};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

bool EPOS4::setShutdown()
{
    QVector<quint16> word{0x9002, 0x6804, 0x6040, 0x0100, 0x0006, 0x0000};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

bool EPOS4::setEnableState()
{
    QVector<quint16> word{0x9002, 0x6804, 0x6040, 0x0100, 0x000f, 0x0000};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

bool EPOS4::setHalt()
{
    QVector<quint16> word{0x9002, 0x6804, 0x6040, 0x0100, 0x010f, 0x0000};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

bool EPOS4::setProfileVelocityMode()
{
    QVector<quint16> word{0x9002, 0x6804, 0x6060, 0x0100, 0x0003, 0x0000};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

// 3571
bool EPOS4::setMaximalProfileVelocity(quint16 value = 3571)
{
    QVector<quint16> word{0x9002, 0x6804, 0x6040, 0x0100, 0xffff, 0x0000};
    word[3]          = value;
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

bool EPOS4::setQuickstopDeceleration(quint16 value = 45)
{
    QVector<quint16> word{0x9002, 0x6804, 0x6085, 0x0100, 0xffff, 0x0000};
    word[3]          = value;
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

bool EPOS4::setProfileAcceleration(quint16 value = 45)
{
    QVector<quint16> word{0x9002, 0x6804, 0x6083, 0x0100, 0xffff, 0x0000};
    word[3]          = value;
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

bool EPOS4::setProfileDeceleration(quint16 value = 45)
{
    QVector<quint16> word{0x9002, 0x6804, 0x6084, 0x0100, 0xffff, 0x0000};
    word[3]          = value;
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

bool EPOS4::setTargetVelocity(quint16 velocity)
{
    QVector<quint16> word{0x9002, 0x6804, 0x60ff, 0x0100, 0xffff, 0x0000};
    word[3]          = velocity;
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

qint32 EPOS4::getActualVelocity(void)
{
    qint32           speed = 0;
    QVector<quint16> word{0x9002, 0x1002, 0x606C, 0x0100};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);

    waitResponse(waitTime);
    if(recvData.length() == 0x0b)
        speed = static_cast<quint16>(recvData.at(6) << 8) + static_cast<quint8>(recvData.at(5));
    else
        speed = -1;

    return speed;
}

bool EPOS4::setProfilePositionMode()
{
    QVector<quint16> word{0x9002, 0x6804, 0x6060, 0x0100, 0x0001, 0x0000};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

bool EPOS4::setProfileVelocity(quint16 value)
{
    QVector<quint16> word{0x9002, 0x6804, 0x6081, 0x0100, 0x0001, 0x0000};
    word[3]          = value;
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

/**
 * @brief
 *  0x9002, 0x6804: OpCode, 0x11; len, 0x03
 *  0x607a: WORD index
 *  0x0100: BYTE SubIndex, 0x01; 0x00, BYTE Node ID
 * @param value
 * @return
 */
bool EPOS4::setTargetPosition(quint32 value)
{
    QVector<quint16> word{0x9002, 0x6804, 0x607a, 0x0100, 0xffff, 0xffff};
    word[3]          = value & 0xffff;
    word[4]          = (value >> 16) & 0xffff;
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

qint32 EPOS4::getActualPosition(void)
{
    qint32 postion = 0;
    //    QVector<quint16> word{0x0260, 0xB001, 0x0030, 0x0000};

    QVector<quint16> word{0x9002, 0x1002, 0x6064, 0x0100};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);

    waitResponse(waitTime);
    if(recvData.length() == 0x0b)
        postion = (static_cast<quint8>(recvData.at(8)) << 24) + (static_cast<quint8>(recvData.at(7)) << 16) + (static_cast<quint8>(recvData.at(6)) << 8) + static_cast<quint8>(recvData.at(5));
    else
        postion = -1;

    return postion;
}

bool EPOS4::setAbsolutePositionStartImmdeitaly()
{
    QVector<quint16> word{0x9002, 0x6804, 0x6040, 0x0100, 0x003f, 0x0000};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

bool EPOS4::setHomeMode()
{
    QVector<quint16> word{0x9002, 0x6804, 0x6060, 0x0100, 0x0006, 0x0000};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    return true;
}

bool EPOS4::startHoming()
{
    QVector<quint16> word{0x9002, 0x6804, 0x6040, 0x0100, 0x001f, 0x0000, 0xac31};
    QByteArray       frame = transmitWord2Byte(word);

    return true;
}

bool EPOS4::stopHoming()
{
    QVector<quint16> word{0x9002, 0x6804, 0x6040, 0x0100, 0x000f, 0x0000, 0xef52};
    QByteArray       frame = transmitWord2Byte(word);

    return true;
}

bool EPOS4::setPositiveSpeed()
{
    QVector<quint16> word{0x9002, 0x6804, 0x6098, 0x0100, 0x0022, 0x0000, 0xf906};
    QByteArray       frame = transmitWord2Byte(word);

    return true;
}
