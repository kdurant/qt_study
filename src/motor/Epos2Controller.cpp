#include "Epos2Controller.h"

/**
 * @brief 设置电机默认工作参数
 * @return
 */
bool EPOS2::init()
{
    // 电机重新上电后，这4步是必须的
    clearFault();
    setShutdown();
    setHalt();
    setPorfileVelocityMode();

    // 这4步是可选的
    setMaximalProfileVelocity(3571);
    setQuickstopDeceleration(45);
    setProfileAcceleration(45);
    setProfileDeceleration(45);
    return true;
}

bool EPOS2::start()
{
    return false;
}

bool EPOS2::stop()
{
  return false;
}

bool EPOS2::run(quint16 speed)
{
  if(!setTargetVelocity(speed))
    return false;
  if(!setEnableState())
    return false;

  return true;
}

/**
 * @brief 将一包完整的数据分成两次发送
 * @param frame
 * @return
 */
bool EPOS2::sendFrameStep1(QByteArray frame)
{
    // step1, send OpCode
    QByteArray data = frame.mid(0, 1);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, 1, data);
    waitResponse(waitTime);
    if(!isResponse4f())
        return false;

    // step2, sendData
    data = frame.mid(1);
    emit sendDataReady(MasterSet::MOTOR_PENETRATE, data.length(), data);
    waitResponse(waitTime);
    if(!isResponse4f00())
        return false;

    return true;
}
/**
 * @brief 数据收尾流程
 * @return
 */
bool EPOS2::sendFrameStep2()
{
    // step 3, 主动发送4f
    send_4f_actively();
    waitResponse(waitTime);
    if(recvData.length() != 0x07)
        return false;

    // step 4, 再次发送4f，结束
    send_4f_actively();
    return true;
}

quint16 EPOS2::calcFieldCRC(quint16 *pDataArray, quint16 numberofWords)
{
    quint16 shifter, c;
    quint16 carry;
    quint16 CRC = 0;

    //Calculate pDataArray Word by Word
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

QVector<quint16> EPOS2::WordPlusCRC(QVector<quint16> word)
{
    word.append(0x0000);  //zeroWord

    quint16  numberOfWords  = word.length();
    quint16 *ptr            = &word[0];
    quint16  crc            = calcFieldCRC(ptr, numberOfWords);
    word[numberOfWords - 1] = crc;
    return word;
}

QByteArray EPOS2::transmitWord2Byte(QVector<quint16> word)
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

bool EPOS2::setDisableState()
{
    QVector<quint16> word{0x1103, 0x6040, 0x0100, 0x0000, 0x0000};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    if(!sendFrameStep1(frame))
        return false;

    if(!sendFrameStep2())
        return false;

    return true;
}

bool EPOS2::clearFault()
{
    QVector<quint16> word{0x1103, 0x6040, 0x0100, 0x0080, 0x0000};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    if(!sendFrameStep1(frame))
        return false;

    if(!sendFrameStep2())
        return false;

    return true;
}

bool EPOS2::setShutdown()
{
    QVector<quint16> word{0x1103, 0x6040, 0x0100, 0x0006, 0x0000};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    if(!sendFrameStep1(frame))
        return false;

    if(!sendFrameStep2())
        return false;

    return true;
}

bool EPOS2::setEnableState()
{
    QVector<quint16> word{0x1103, 0x6040, 0x0100, 0x000f, 0x0000};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    if(!sendFrameStep1(frame))
        return false;

    if(!sendFrameStep2())
        return false;

    return true;
}

bool EPOS2::setHalt()
{
    QVector<quint16> word{0x1103, 0x6040, 0x0100, 0x010f, 0x0000};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    if(!sendFrameStep1(frame))
        return false;

    if(!sendFrameStep2())
        return false;

    return true;
}

bool EPOS2::setPorfileVelocityMode()
{
    QVector<quint16> word{0x1103, 0x6060, 0x0100, 0x0003, 0x0000};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    if(!sendFrameStep1(frame))
        return false;

    if(!sendFrameStep2())
        return false;

    return true;
}

// 3571
bool EPOS2::setMaximalProfileVelocity(quint16 value = 3571)
{
    QVector<quint16> word{0x1103, 0x6040, 0x0100, 0xffff, 0x0000};
    word[3]          = value;
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    if(!sendFrameStep1(frame))
        return false;

    if(!sendFrameStep2())
        return false;

    return true;
}

bool EPOS2::setQuickstopDeceleration(quint16 value = 45)
{
    QVector<quint16> word{0x1103, 0x6085, 0x0100, 0xffff, 0x0000};
    word[3]          = value;
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    if(!sendFrameStep1(frame))
        return false;

    if(!sendFrameStep2())
        return false;

    return true;
}

bool EPOS2::setProfileAcceleration(quint16 value = 45)
{
    QVector<quint16> word{0x1103, 0x6083, 0x0100, 0xffff, 0x0000};
    word[3]          = value;
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    if(!sendFrameStep1(frame))
        return false;

    if(!sendFrameStep2())
        return false;

    return true;
}

bool EPOS2::setProfileDeceleration(quint16 value = 45)
{
    QVector<quint16> word{0x1103, 0x6084, 0x0100, 0xffff, 0x0000};
    word[3]          = value;
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    if(!sendFrameStep1(frame))
        return false;

    if(!sendFrameStep2())
        return false;

    return true;
}

bool EPOS2::setTargetVelocity(quint16 velocity)
{
    QVector<quint16> word{0x1103, 0x60ff, 0x0100, 0xffff, 0x0000};
    word[3]          = velocity;
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    if(!sendFrameStep1(frame))
        return false;

    if(!sendFrameStep2())
        return false;

    return true;
}

qint32 EPOS2::getActualVelocity()
{
    qint32           speed = 0;
    QVector<quint16> word{0x1001, 0x606C, 0x0100};
    word             = WordPlusCRC(word);
    QByteArray frame = transmitWord2Byte(word);

    if(!sendFrameStep1(frame))
      return false;

    // step 3, 主动发送4f
    send_4f_actively();
    waitResponse(waitTime);
    if(recvData.length() == 0x0b)
      speed = static_cast<quint16>( recvData.at(6)<<8) + static_cast<quint8>(recvData.at(5));
    else
        speed = -1;

    // step 4, 再次发送4f，结束
    send_4f_actively();

    return speed;
}

qint32 EPOS2::getActualPosition()
{
  qint32           postion = 0;
  QVector<quint16> word{0x0260, 0xB001, 0x0030, 0x0000};
  word             = WordPlusCRC(word);
  QByteArray frame = transmitWord2Byte(word);

  if(!sendFrameStep1(frame))
    return false;

  // step 3, 主动发送4f
  send_4f_actively();
  waitResponse(waitTime);
  if(recvData.length() == 0x0b)
    postion = static_cast<quint16>( recvData.at(6)<<8) + static_cast<quint8>(recvData.at(5));
  else
    postion = -1;

  // step 4, 再次发送4f，结束
  send_4f_actively();

  return postion;
  return 0;
}
