/* =============================================================================
# FileName    :	ms5837.h
# Author      :	author
# Email       :	email@email.com
# Description :	ms5837传感器的数据通过解算板的uart上传
                字符串格式： T=XX.XXD=XX.XX\r\n
# Version     :	1.0
# LastChange  :	2022-09-26 14:52:55
# ChangeLog   :
============================================================================= */
#ifndef MS5837_H
#define MS5837_H
#include <QtCore>

class MS5837 : public QObject
{
    Q_OBJECT
public:
    MS5837() = default;
    struct Info
    {
        double temperature;
        double depth;
    };
    Info info;

signals:
    void sendMS5837Result(Info info);
public slots:
    void parserFrame(QByteArray data)
    {
        if(data.size() != 16)
            return;

        info.temperature = data.mid(2, 5).toDouble(nullptr);
        info.depth       = data.mid(9, 5).toDouble(nullptr);

        emit sendMS5837Result(info);
    }
};
#endif
