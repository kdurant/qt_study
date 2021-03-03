#include "SaveWave.h"

/**
 * @brief 读取指定unit地址的数据
 * @param unitAddr
 * @param ret, 返回的数据
 * @return 
 */
bool SaveWave::readDiskUnit(qint32 unitAddr, QByteArray &ret)
{
    QByteArray frame = BspConfig::int2ba(unitAddr);
    emit sendDataReady(MasterSet::READ_SSD_UNIT, 4, frame);
    QThread::msleep(1);

    QEventLoop waitLoop; // 等待响应数据，或者1000ms超时
    connect(timer, &QTimer::timeout, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();

    if (allData.size() != 64)
        return false;

    if (ProtocolDispatch::getPckNum(allData[0]) != 0)
        return false;

    if (ProtocolDispatch::getPckNum(allData[63]) != 63)
        return false;

    for (int i = 0; i < 64; i++) {
        if (ProtocolDispatch::getPckNum(allData[i]) != i) {
            return false;
        }
    }
    ret = allData[0];
    allData.clear();
    timer->stop();
    return true;
}

/**
 * @brief 查询下一个可用的文件地址
 * @param startUnit
 * @param fileInfo
 * @return 
 */
bool SaveWave::inquireSpace(qint32 startUnit, ValidFileInfo &fileInfo)
{
    bool status = true;
    QByteArray fileName;
    QByteArray filePos;
    qint32 unit = startUnit;
    while (status) {
        if (!readDiskUnit(unit++, fileName)) // 读操作失败
            return false;

        if (fileName.mid(24+0, 8) == fileName.mid(24+8, 8)) //文件名内容错误
            return false;

        if (!readDiskUnit(unit++, filePos)) // 读操作失败
            return false;
        if (filePos.mid(24+0, 8) == filePos.mid(24+8, 8)) //文件位置内容错误
            return false;

        auto swapByteOrder = [](QByteArray &ba){
          for(int i = 0; i< ba.length(); i+=4)
          {
            char c0 = ba.at(i);
            char c1 = ba.at(i+1);
            char c2 = ba.at(i+2);
            char c3 = ba.at(i+3);

            ba[i] = c3;
            ba[i+1] = c2;
            ba[i+2] = c1;
            ba[i+3] = c0;
          }

        };
        QByteArray name = fileName.mid(24, 252);
        QByteArray startUnit = filePos.mid(24, 4);
        QByteArray endUnit = filePos.mid(28, 4);
        // 上面得到数据因为大小端的问题，在每4个字节内，需要交换字节序
        swapByteOrder(name);
        swapByteOrder(startUnit);
        swapByteOrder(endUnit);

        fileInfo.name = name;
        fileInfo.fileUnit = unit-2;
        fileInfo.startUnit = BspConfig::ba2int(startUnit);
        fileInfo.endUnit = BspConfig::ba2int(endUnit);
    }
    return true;
}

bool SaveWave::setSaveFileName(QByteArray &name)
{
    emit sendDataReady(MasterSet::READ_SSD_UNIT, 4, name);
}
