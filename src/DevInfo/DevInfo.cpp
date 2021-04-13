#include "DevInfo.h"

bool DevInfo::getSysPara(QVector<DevInfo::ParaInfo>& ret)
{
    QByteArray frame = BspConfig::int2ba(0x01);
    emit       sendDataReady(MasterSet::SYS_INFO, 4, frame);

    QEventLoop waitLoop;  // 等待响应数据，或者1000ms超时
    connect(this, &DevInfo::responseDataReady, &waitLoop, &QEventLoop::quit);
    QTimer::singleShot(10, &waitLoop, &QEventLoop::quit);
    waitLoop.exec();
    if(recvData.size() != 256)  // 没有收到响应数据
    {
        ret = sysPara;
        return false;
    }

    for(int i = 0; i < sysPara.length(); i++)
    {
        sysPara[i].value = recvData.mid(sysPara[i].offset, sysPara[i].len);
    }
    recvData.clear();
    ret = sysPara;

    return true;
}
