#ifndef NETSTUDENT_H
#define NETSTUDENT_H
#include<QObject>
#include<QTcpSocket>
#include"filereceiver.h"

class NetStudent:public QObject
{
    Q_OBJECT

private:
    int stuId;      //学生id
    QString computerName;   //计算机名
    QTcpSocket *socket;     //教师端与学生关联的套接字
    FileReceiver *frcver;   //文件接收
    char macAddr[6];        //学生端mac地址
    bool isOnLine;          //学生是否在线

public:
    explicit NetStudent(QObject *parent = nullptr);

    //NetStudent();

    int getStuId() const;
    void setStuId(int newStuId);
    QTcpSocket *getSocket() const;
    void setSocket(QTcpSocket *newSocket);
    void setMacAddr(const char mac[]);
    void getMacAddr(char mac[]);
    const QString &getComputerName() const;
    void setComputerName(const QString &newComputerName);

public slots:
    void slotReadyRead();
    void slotErrorOccurred(QAbstractSocket::SocketError socketError);
signals:
    void sigDisconnected(int sId);
};

#endif // NETSTUDENT_H
