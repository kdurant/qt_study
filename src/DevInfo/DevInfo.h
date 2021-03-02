#ifndef DEVINFO_H
#define DEVINFO_H
#include <QtCore>

#include "protocol.h"
class DevInfo
{
public:
    struct SysParaOffset
    {
        QString name;
        qint32  offset;
    };

    struct SysParaInfo
    {
      QString fpgaVer;
      quint32 laserFreq;
      quint32 adSampleLen;
      quint32 previewRatio;
      quint32 cameraFreq;
      quint32 isStoringWaveform;
      quint32 isADCapturing;
      quint32 isCameraCapturing;
      quint32 isSSDLinking;

    };

    DevInfo()
    {
        sysPara.append({"FPGA版本:", 248});
        sysPara.append({"激光重频:", 0});
        sysPara.append({"AD采样长度:", 4});
        sysPara.append({"AD预览抽样率:", 8});
        sysPara.append({"相机触发频率:", 12});
        sysPara.append({"波形存储状态:", 16});
        sysPara.append({"AD采集状态:", 20});
        sysPara.append({"相机触发状态:", 24});
        sysPara.append({"SSD链接状态:", 28});
        sysPara.append({"保留未用:", 36});
        sysPara.append({"保留未用:", 40});

        sysPara.append({"第一段起始位置:", 60});
        sysPara.append({"第一段采样长度:", 64});
        sysPara.append({"第二段起始位置:", 68});
        sysPara.append({"第二段采样长度:", 72});
        sysPara.append({"和阈值:", 76});
        sysPara.append({"值阈值:", 80});
    }

    SysParaInfo &getSysPara(void);


signals:
    void sendDataReady(qint32 command, qint32 data_len, QByteArray& data);  // 需要发送的数据已经准备好

    void responseDataReady(void);  // 接收到响应数据
private:
    QByteArray recvData;
    QVector<SysParaOffset> sysPara;

public slots:

    void setNewData(QByteArray& data)
    {
      recvData      = data;
      emit responseDataReady();
    }
};

#endif
