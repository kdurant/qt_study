#include "Epos2Controller.h"

/**
 * @brief 将uint16数据转换到QByteArray中
 * @param word
 * @return
 */
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

qint32 EPOS2::getActualVelocity()
{
    QVector<quint16> word;
    word.append(0x1001);
    word.append(0x606C);
    word.append(0x0100);
    word = WordPlusCRC(word);

    // step1, send OpCode
    QByteArray frame = transmitWord2Byte(word);
    QByteArray data  = frame.mid(0, 1);
    emit       sendDataReady(MasterSet::MOTOR_PENETRATE, 1, data);
    waitResponse(1000);
    if (!isResponse4f())
        return -1;

    // step2, sendData
    data = frame.mid(1);
    emit sendDataReady(MasterSet::MOTOR_PENETRATE, 1, data);
    waitResponse(1000);
    if (!isResponse4f00())
        return -1;

    // step 3
    send_4f_actively();
    waitResponse(1000);
    if (recvData.length() == 0x0b) {
        recvData.mid(4, 2);
    }

    return false;
}
