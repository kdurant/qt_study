#ifndef DEVINFO_H
#define DEVINFO_H

#include <QtCore>
#include "protocol.h"
class DevInfo : public QObject
{
    Q_OBJECT

public:
    struct ParaInfo
    {
        QString    name;
        QByteArray value;
        quint32    offset;
        quint32    len;
    };

    //    struct SysParaInfo
    //    {
    //        QString fpgaVer;
    //        quint32 laserFreq;
    //        quint32 adSampleLen;
    //        quint32 previewRatio;
    //        quint32 cameraFreq;
    //        quint32 isStoringWaveform;
    //        quint32 isADCapturing;
    //        quint32 isCameraCapturing;
    //        quint32 isSSDLinking;
    //    };

    DevInfo()
    {
        sysPara.append({"FPGA版本       : ", {}, 248, 8});
        sysPara.append({"激光重频       : ", {}, 0, 4});
        sysPara.append({"AD采样长度     : ", {}, 4, 4});
        sysPara.append({"AD预览抽样率   : ", {}, 8, 4});
        //        sysPara.append({"相机触发频率   : ", {}, 12, 4});
        sysPara.append({"波形存储状态   : ", {}, 16, 4});
        sysPara.append({"AD采集状态     : ", {}, 20, 4});
        //        sysPara.append({"相机触发状态   : ", {}, 24, 4});
        sysPara.append({"SSD链接状态    : ", {}, 28, 4});
        sysPara.append({"sata读写状态机    : ", {}, 32, 1});
        sysPara.append({"文件读写状态机       : ", {}, 33, 3});
        //        sysPara.append({"保留未用       : ", {}, 36, 4});

        sysPara.append({"第一段起始位置 : ", {}, 60, 4});
        sysPara.append({"第一段采样长度 : ", {}, 64, 4});
        sysPara.append({"第二段起始位置 : ", {}, 68, 4});
        sysPara.append({"第二段采样长度 : ", {}, 72, 4});
        sysPara.append({"和阈值         : ", {}, 76, 4});
        sysPara.append({"值阈值         : ", {}, 80, 4});
    }

    bool getSysPara(QVector<DevInfo::ParaInfo>& ret);

signals:
    void sendDataReady(qint32 command, qint32 data_len, QByteArray& data);  // 需要发送的数据已经准备好
    void responseDataReady(void);
    // 接收到响应数据

private:
    QByteArray        recvData;
    QVector<ParaInfo> sysPara;

public slots:
    void setNewData(QByteArray& data)
    {
        recvData = data;
        emit responseDataReady();
    }
};

#endif
