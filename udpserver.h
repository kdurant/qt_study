#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QUdpSocket>


class UdpServer
{
public:
    QUdpSocket *udpSocket;

    UdpServer();

    void initSocket(QHostAddress &addr, quint16 port);

};

#endif // UDPSERVER_H
