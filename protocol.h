#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <QString>
#include <QByteArray>
#include <QVector>

class Protocol
{
    QString head;
    qint32 cmd_num;
    qint32 command;
    qint32 pck_num;
    qint32 data_len;
    QVector<int> data;
    qint32 checsum;

public:
    Protocol();
    ~Protocol();

    QByteArray encode(qint32 command, qint32 data_len, QVector<int> &data);


};

#endif // PROTOCOL_H
