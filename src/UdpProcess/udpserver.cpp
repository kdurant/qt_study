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

}
