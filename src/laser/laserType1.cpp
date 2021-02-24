#include "laserType1.h"

bool LaserType1::setMode(LaserController::OpenMode mode)
{
    quint8     data[8] = {0x55, 0xAA, 0x01, 0x01, 0x00, 0x01, 0x33, 0xcc};
    QByteArray array;
    for(int i = 0; i < 8; i++)
    {
        array.append(data[i] & 0xff);
    }
    array[5] = mode;

    emit sendDataReady(array);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    connect(this, &LaserType1::responseDataReady, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(1000, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();

    return true;
}

bool LaserType1::open()
{
    quint8     data[8] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x01, 0x33, 0xcc};
    QByteArray array;
    for(int i = 0; i < 8; i++)
    {
        array.append(data[i] & 0xff);
    }
    return true;
}

bool LaserType1::close()
{
    quint8     data[8] = {0x55, 0xAA, 0x01, 0x02, 0x00, 0x02, 0x33, 0xcc};
    QByteArray array;
    for(int i = 0; i < 8; i++)
    {
        array.append(data[i] & 0xff);
    }
    return true;
}

bool LaserType1::setPower(qint8 power)
{
    QByteArray array;
    array.append((char)0x55);
    array.append((char)0xAA);
    array.append((char)0x01);
    array.append((char)0x07);
    array.append((char)0x00);

    switch(power)
    {
        case 0:
            array.append((char)0x64);
            break;
        case 1:
            array.append((char)0x32);
            break;
        case 2:
            array.append((char)0x00);
            break;
        default:
            break;
    }

    array.append((char)0x33);
    array.append((char)0xCC);

    return true;
}

bool LaserType1::setLD2DlyTime(quint16 value)
{
    quint8     data[4] = {0x55, 0xAA, 0x01, 0x07};
    QByteArray array;
    for(int i = 0; i < 4; i++)
    {
        array.append(data[i] & 0xff);
    }
    array.append((quint8)((0xff00 & value) >> 8));
    array.append((quint8)(0x00ff & value));
    array.append(0x33);
    array.append(0xcc);

    return true;
}
