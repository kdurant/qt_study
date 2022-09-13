#include "Epos4Controller.h"

MotorController::MOTOR_STATUS EPOS4::init()
{
    clearFault();
    setShutdown();
    setEnableState();
    setHalt();
    setProfileVelocityMode();
    setMaxProfileVelocity(3571);
    //    setProfileVelocity();
    setQuickstopDeceleration(45);
    setProfileAcceleration(45);
    setProfileDeceleration(45);
}

MotorController::MOTOR_STATUS EPOS4::run(quint16 speed)
{
}

qint32 EPOS4::getActualVelocity()
{
    return 0;
}

qint32 EPOS4::getActualPosition()
{
    return 0;
}

MotorController::MOTOR_STATUS EPOS4::moveToPosition(double postion, int direct)
{

}

bool EPOS4::moveToHome()
{
    if(!clearFault())
        return false;
    setHomeMode();
    //    setPositiveSpeed();
    //    setShutdown();
    //    setEnableState();
    //    setHalt();
    //    startHoming();
    return true;
}

bool EPOS4::moveFixSpeed(quint32 speed)
{

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

QVector<uint8_t> EPOS4::WordPlusCRC(QVector<uint8_t> &word)
{
    uint16_t  numberOfWords = word.length();
    uint16_t *ptr           = (uint16_t *)&word[0];
    uint16_t  crc           = calcFieldCRC(ptr, numberOfWords);
    word.append((uint8_t)((crc & 0xff00) >> 8));
    word.append((uint8_t)((crc & 0xff00) >> 8));
    return word;
}

QByteArray EPOS4::transmitWord2Byte(const QVector<uint8_t> &word)
{
    QByteArray array;
    array.resize(word.length() * 2);
    for(auto &item : word)
        array.append(item);
    return array;
}

bool EPOS4::clearFault()
{
    packetControlWordFrame(0x6040, 0x01, 0x00, 0x0080);
    WordPlusCRC(frame);
    QByteArray packet = transmitWord2Byte(frame);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, packet.length(), packet);

    return true;
}

bool EPOS4::setShutdown()
{
    packetControlWordFrame(0x6040, 0x01, 0x00, 0x0006);
    WordPlusCRC(frame);
    QByteArray packet = transmitWord2Byte(frame);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, packet.length(), packet);
    return true;
}

bool EPOS4::setEnableState()
{
    packetControlWordFrame(0x6040, 0x01, 0x00, 0x000f);
    WordPlusCRC(frame);
    QByteArray packet = transmitWord2Byte(frame);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, packet.length(), packet);
    return true;
}

bool EPOS4::setHalt()
{
    packetControlWordFrame(0x6040, 0x01, 0x00, 0x010f);
    WordPlusCRC(frame);
    QByteArray packet = transmitWord2Byte(frame);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, packet.length(), packet);
    return true;
}

bool EPOS4::setProfileVelocityMode()
{
    packetControlWordFrame(0x6060, 0x01, 0x00, 0x0003);
    WordPlusCRC(frame);
    QByteArray packet = transmitWord2Byte(frame);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, packet.length(), packet);
    return true;
}

bool EPOS4::setMaxProfileVelocity(uint16_t value)
{
    packetControlWordFrame(0x607f, 0x01, 0x00, value);
    WordPlusCRC(frame);
    QByteArray packet = transmitWord2Byte(frame);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, packet.length(), packet);
    return true;
}

bool EPOS4::setQuickstopDeceleration(quint16 value)
{
    packetControlWordFrame(0x6085, 0x01, 0x00, value);
    WordPlusCRC(frame);
    QByteArray packet = transmitWord2Byte(frame);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, packet.length(), packet);
    return true;
}

bool EPOS4::setProfileAcceleration(quint16 value)
{
    packetControlWordFrame(0x6083, 0x01, 0x00, value);
    WordPlusCRC(frame);
    QByteArray packet = transmitWord2Byte(frame);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, packet.length(), packet);
    return true;
}

bool EPOS4::setProfileDeceleration(quint16 value)
{
    packetControlWordFrame(0x6084, 0x01, 0x00, value);
    WordPlusCRC(frame);
    QByteArray packet = transmitWord2Byte(frame);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, packet.length(), packet);
    return true;
}

bool EPOS4::setHomeMode()
{
    packetControlWordFrame(0x6060, 0x01, 0x00, 0x0006);
    WordPlusCRC(frame);
    QByteArray packet = transmitWord2Byte(frame);

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, packet.length(), packet);
    return true;
};
