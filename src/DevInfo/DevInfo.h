#ifndef DEVINFO_H
#define DEVINFO_H
#include <QString>

class DevInfo
{
public:
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
};

#endif
