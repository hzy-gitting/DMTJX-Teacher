#ifndef RTCP_H
#define RTCP_H
#include<QList>
#include<QTcpSocket>
#include"netstudent.h"
#include<QTcpServer>
#include<QObject>

class RTCP:public QObject
{
    Q_OBJECT

private:
    static RTCP *pInst;

   QList<NetStudent*> stuList;
private:
   RTCP(QHostAddress ipAddr,quint16 port);
   void init(QHostAddress ipAddr,quint16 port);
   QTcpServer *rtcpSrv;

public:

    static RTCP * getInstance();    //获取单例
    static bool start(QHostAddress ipAddr,quint16 port);

    bool sendFileRcvCommand(int stuId,QList<QString> fileList,QList<qint64> fileSizeList);

    bool sendFileData(int sId, QByteArray data);
    bool sendMessage(int sId, QByteArray msg);

    bool sendFileRcvCommand(QList<QString> fileList,QList<qint64> fileSizeList);

    bool sendFileData( QByteArray data);
    bool sendMessage( QByteArray msg);


    NetStudent *findStudentById(int stuId);
    bool sendShutdownCommand(bool bRestart=false);
    bool sendShutdownCommand(int sId,bool bRestart=false);
    bool sendStopScreenShareCommand(int sId);
    bool sendStopScreenShareCommand();
    bool sendStartcreenShareCommand(int sId);
    bool sendStartcreenShareCommand();
private slots:
    void newConn();
signals:
    void newStudentConnection(int sId,QHostAddress ip,qint32 port,char macAddr[]);
    void sigStuDisconnected(int sId);
    void sigNewMessage(const QByteArray&msgContent,const QHostAddress &senderAddr);   //rtcp新消息信号
};

#endif // RTCP_H
