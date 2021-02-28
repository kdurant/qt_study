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

        if (fileName.contains(QByteArray(100, 0xee))) //文件名内容错误
            return false;

        if (!readDiskUnit(unit++, filePos)) // 读操作失败
            return false;
        if (filePos.mid(0, 8).contains(QByteArray(4, 0xdd))) //文件名内容错误
            return false;

        fileInfo.name = fileName.mid(0, 252);
        fileInfo.startUnit = BspConfig::ba2int(filePos.mid(0, 4));
        fileInfo.endUnit = BspConfig::ba2int(filePos.mid(4, 4));
    }
    return true;
}

bool SaveWave::setSaveFileName(QByteArray &name)
{
    emit sendDataReady(MasterSet::READ_SSD_UNIT, 4, name);
}
