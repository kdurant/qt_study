#include "udpserver.h"

#include <QMessageBox>

UdpServer::UdpServer()
{
}

void UdpServer::initSocket(QHostAddress &addr, quint16 port)
{
    udpSocket = new QUdpSocket;
    if(~udpSocket->bind(addr, port))
        QMessageBox::warning(NULL, "警告", "UDP绑定失败!");
}

void UdpServer::processPendingDatagram()
{
    QByteArray datagram;
    QByteArray data;
    int        len;
    while(udpSocket->hasPendingDatagrams())
    {
        len = udpSocket->pendingDatagramSize();
        datagram.resize(len);
        udpSocket->readDatagram(datagram.data(), datagram.size());
        qDebug() << datagram.data();
        //        data = datagram.toHex();
        //        if(data.mid(COMMAND_POS, COMMAND_LEN) == "80100002")
        //            m_Update.ReadData = data;
        //            qDebug() <<
    }
}
