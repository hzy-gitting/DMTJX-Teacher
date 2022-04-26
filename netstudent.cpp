#include "netstudent.h"

int NetStudent::getStuId() const
{
    return stuId;
}

void NetStudent::setStuId(int newStuId)
{
    stuId = newStuId;
}

QTcpSocket *NetStudent::getSocket() const
{
    return socket;
}

void NetStudent::setSocket(QTcpSocket *newSocket)
{
    socket = newSocket;
}

NetStudent::NetStudent()
{
    stuId = -2;
    socket = nullptr;
}
