#include "protocol.h"

Protocol::Protocol()
{
    cmd_num = 0;
}

QByteArray Protocol::encode(qint32 command, qint32 data_len, QVector<int> &data)
{
    QByteArray frame("AA555AA5AA555AA5");
    frame.append(QByteArray::fromHex(cmd_num++));
    frame.append(QByteArray::fromHex(command));
    frame.append(QByteArray::fromHex(pck_num));
    frame.append(QByteArray::fromHex(data_len));
}
