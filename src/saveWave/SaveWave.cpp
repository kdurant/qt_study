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
    emit       sendDataReady(MasterSet::READ_SSD_UNIT, 4, frame);
    QThread::msleep(1);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    timer->start();
    connect(timer, &QTimer::timeout, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();
    if(allData.size() == 0)
        return false;

    if(allData.size() != 64)
    {
        allData.clear();
        return false;
    }
    // 发现包序号存在不连续的情况，检查前面几个包，简单判断下
    for(int i = 0; i < 8; i++)
    {
        if(ProtocolDispatch::getPckNum(allData[i]) != i)
        {
            allData.clear();
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
 * 1. 文件名内容错误，可能是文件搜寻确实结束了，新的位置没有文件;
 * 2. 文件开始、结束地址错误，可能有因为sata状态机错误，导致信息没有被正确写入
 * @param startUnit
 * @param fileInfo
 * @return 
 */
bool SaveWave::inquireSpace(qint32 startUnit, ValidFileInfo &fileInfo)
{
    bool       status = true;
    QByteArray fileName;
    QByteArray filePos;
    qint32     unit    = startUnit;
    fileInfo.fileUnit  = -2;
    fileInfo.startUnit = 0x5000 - 2;
    fileInfo.endUnit   = 0x5000 - 1;
    fileList.clear();
    auto swapByteOrder = [](QByteArray &ba) {
        for(int i = 0; i < ba.length(); i += 4)
        {
            char c0 = ba.at(i);
            char c1 = ba.at(i + 1);
            char c2 = ba.at(i + 2);
            char c3 = ba.at(i + 3);

            ba[i]     = c3;
            ba[i + 1] = c2;
            ba[i + 2] = c1;
            ba[i + 3] = c0;
        }
    };
    while(status)
    {
        // 保存文件名的unit中，默认追加的数据是0xee
        readDiskUnit(unit++, fileName);
        // 保存文件信息的unit中，默认追加的数据是0xdd
        readDiskUnit(unit++, filePos);

        QByteArray name      = fileName.mid(24, 252);
        QByteArray startUnit = filePos.mid(24, 4);
        QByteArray endUnit   = filePos.mid(28, 4);
        // 上面得到数据因为大小端的问题，在每4个字节内，需要交换字节序
        swapByteOrder(name);
        swapByteOrder(startUnit);
        swapByteOrder(endUnit);

        // 没有从硬盘中读取到数据
        if(name.length() == 0 || filePos.length() == 0)
        {
            fileInfo.status = FILE_INFO_NONE;
            return false;
        }

        // 文件名和文件位置信息都无效， 说明本次检查没有任何有效数据
        // 返回上次检查得到的文件信息即可
        if(name.mid(0, 8) == name.mid(8, 8) && startUnit == endUnit)
        {
            fileInfo.status = FILE_INFO_FULL;
            return true;
        }
        fileInfo.name      = name;
        fileInfo.fileUnit  = unit - 2;
        fileInfo.startUnit = BspConfig::ba2int(startUnit);
        fileInfo.endUnit   = BspConfig::ba2int(endUnit);

        fileList.append(fileInfo);
        emit fileDataReady(fileInfo);

        // 文件名有效，但文件信息无效，说明文件信息没有被正确写入到SSD中，底层状态机异常
        if(name.mid(0, 8) != name.mid(8, 8) && startUnit == endUnit)
        {
            fileInfo.status = FILE_POSITION_ERROR;
            return false;
        }
    }
    return true;
}

/**
 * @brief 设置存储文件的名称
 * @return
 */
bool SaveWave::setSaveFileName(quint32 unit, QString &name)
{
    QByteArray frame;
    frame.append(BspConfig::int2ba(unit));
    frame.append(name.toUtf8());
    emit sendDataReady(MasterSet::SET_STORE_FILE_NAME, frame.length(), frame);
    return true;
}

bool SaveWave::setSaveFileAddr(quint32 unit)
{
    QByteArray frame;
    frame.append(BspConfig::int2ba(unit));
    emit sendDataReady(MasterSet::SET_WRITE_DATA_UNIT, 4, frame);
    return true;
}

bool SaveWave::enableStoreFile(quint32 status)
{
    QByteArray frame;
    frame.append(BspConfig::int2ba(status));
    emit sendDataReady(MasterSet::STORE_FILE_STATUS, 4, frame);
    return true;
}
