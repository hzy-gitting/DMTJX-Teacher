#ifndef NETSTUDENT_H
#define NETSTUDENT_H

#include<QTcpSocket>

class NetStudent
{
private:
    int stuId;
    QTcpSocket *socket;
public:
    NetStudent();
    int getStuId() const;
    void setStuId(int newStuId);
    QTcpSocket *getSocket() const;
    void setSocket(QTcpSocket *newSocket);
};

#endif // NETSTUDENT_H
