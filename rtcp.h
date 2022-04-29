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
   RTCP();
   void init();
   QTcpServer *rtcpSrv;

public:

    static RTCP * getInstance();    //获取单例

    bool sendFileRcvCommand(int stuId,QList<QString> fileList,QList<qint64> fileSizeList);

    NetStudent *findStudentById(int stuId);
    static void start();
    bool sendFileData(int sId, QByteArray data);
private slots:
    void newConn();
signals:
    void newStudentConnection(int sId,QHostAddress ip,qint32 port,char macAddr[]);
};

#endif // RTCP_H
