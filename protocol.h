#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <QString>
#include <QByteArray>
#include <QVector>
#include <QQueue>

#include <QDebug>
#include "bsp_config.h"

typedef struct _data
{
    qint32  first_start_pos;
    qint32  first_len;
    QString first_data;

    qint32  second_start_pos;
    qint32  second_len;
    QString second_data;

} AD_Data;

class Protocol
{
    QString      head;
    qint32       cmd_num;
    qint32       command;
    qint32       pck_num;
    qint32       data_len;
    QVector<int> data;
    qint32       checsum;

    QString ad_single_data;

    AD_Data ad_data[4];

public:
    Protocol();
    ~Protocol();

    QByteArray encode(qint32 command, qint32 data_len, qint32 data);

    // 从队列里找出一次采集的数据，存放到一起
    QString &get_single_ad_data(QQueue<QString> &frame);
    AD_Data &get_channal_data(qint32 number);
};

#endif  // PROTOCOL_H
