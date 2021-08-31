#include "PusiController.h"

bool PusiCOntroller::turnStepsByNum(qint8 cAddr, qint32 nSteps)
{
    if(nSteps < 1)
        nSteps = 1;
    if(nSteps > 0x7fffffff)
        nSteps = 0x7fffffff;
    QByteArray frame;
    frame.append(0xA5);  //固定包头
    frame.append(cAddr);
    frame.append(0x73);
    frame.append(nSteps);
    frame.append(nSteps >> 8);
    frame.append(nSteps >> 16);
    frame.append(nSteps >> 24);
    frame.append(calcFieldCRC(frame));

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);

    return true;
}

void PusiCOntroller::clearExit1MarkBit(quint8 addr)
{
    QByteArray frame;
    frame.append(0xA5);
    frame.append(addr);
    frame.append(0x62);
    frame.append(char(0));
    frame.append(char(0));
    frame.append(char(0));
    frame.append(char(0));
    frame.append(calcFieldCRC(frame));

    emit sendDataReady(MasterSet::MOTOR_PENETRATE, frame.length(), frame);
}
