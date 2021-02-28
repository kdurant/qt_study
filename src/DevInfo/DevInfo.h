#ifndef DEVINFO_H
#define DEVINFO_H
#include <QtCore>

class DevInfo
{
public:
    struct SysPara
    {
        QString name;
        qint32  offset;
    };

    DevInfo()
    {
        fpgaVer        = "0.01";
        devType        = "xxxx";
        laserFreq      = 0;
        totalSampleLen = 0;
        previewRatio   = 0;
        firstPos       = 0;
        firstLen       = 0;
        secondPos      = 0;
        secondLen      = 0;
        compressLen    = 0;
        compressRatio  = 0;

        sysPara.append({"FPGA版本:", 248});
        sysPara.append({"激光重频:", 0});
        sysPara.append({"AD采样长度:", 4});
        sysPara.append({"AD预览抽样率:", 8});
        sysPara.append({"相机触发频率:", 12});
        sysPara.append({"波形存储状态:", 16});
        sysPara.append({"波形存储状态:", 16});
        sysPara.append({"AD采集状态:", 20});
        sysPara.append({"相机触发状态:", 24});
        sysPara.append({"SSD链接状态:", 28});
        sysPara.append({"SSD链接状态:", 32});
        sysPara.append({"保留未用:", 36});
        sysPara.append({"保留未用:", 40});

        sysPara.append({"第一段起始位置:", 60});
        sysPara.append({"第一段采样长度:", 64});
        sysPara.append({"第二段起始位置:", 68});
        sysPara.append({"第二段采样长度:", 72});
        sysPara.append({"和阈值:", 76});
        sysPara.append({"值阈值:", 80});
    }

    void setFPGAVer(QByteArray ver)
    {
        fpgaVer = ver;
    }
    QString getFPGAVer() const
    {
        return fpgaVer;
    }

    void setLaserFreq(int freq)
    {
        laserFreq = freq;
    }

    int getLaserFreq() const
    {
        return laserFreq;
    }
    void setTotalSampleLen(int len)
    {
        totalSampleLen = len;
    }

    int getTotalSampleLen() const
    {
        return totalSampleLen;
    }
    void setPreviewRatio(int ratio)
    {
        previewRatio = ratio;
    }

    int getPreviewRatio() const
    {
        return previewRatio;
    }

    void setFirstPos(int pos)
    {
        firstPos = pos;
    }
    int getFirstPos() const
    {
        return firstPos;
    }

    void setFirstLen(int len)
    {
        firstLen = len;
    }
    int getFirstLen() const
    {
        return firstLen;
    }

    void setSecondPos(int pos)
    {
        secondPos = pos;
    }
    int getSecondPos() const
    {
        return secondPos;
    }

    void setSecondLen(int len)
    {
        secondLen = len;
    }
    int getSecondLen() const
    {
        return secondLen;
    }

    void setCompressLen(int len)
    {
        compressLen = len;
    }

    int getCompressRatio() const
    {
        return compressRatio;
    }

    QVector<SysPara> &getSysPara(void) { return sysPara; }

private:
    QString fpgaVer;
    QString devType;
    int     laserFreq;
    int     totalSampleLen;
    int     previewRatio;
    int     firstPos;
    int     firstLen;
    int     secondPos;
    int     secondLen;
    int     compressLen;
    int     compressRatio;

    QVector<SysPara> sysPara;
};

#endif
