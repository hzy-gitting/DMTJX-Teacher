#include "rtcp.h"
#include<QDataStream>
#include<QTcpServer>
#include<WinSock2.h>

RTCP * RTCP::pInst = nullptr;

//远程教学控制协议初始化
void RTCP::init()
{
    //服务器创建
    rtcpSrv = new QTcpServer;
    //服务器开始监听
    if(!rtcpSrv->listen(QHostAddress::Any,8999)){
        qDebug()<<"rtcp服务器监听失败！";
    }
    connect(rtcpSrv,&QTcpServer::newConnection,this,&RTCP::newConn);

}

RTCP::RTCP()
{
    init();
}

void RTCP::start(){
    if(pInst == nullptr){
        pInst = new RTCP;
    }
}
RTCP *RTCP::getInstance()
{
    if(pInst == nullptr){
        pInst = new RTCP;
    }
    return pInst;
}
void RTCP::newConn(){
    static int sId = 1;
    if(rtcpSrv->hasPendingConnections()){
        QTcpSocket *s = rtcpSrv->nextPendingConnection();
        SOCKET nativeSkt = s->socketDescriptor();
        char chOpt = 1;
        int ret = setsockopt(nativeSkt,IPPROTO_TCP,TCP_NODELAY,&chOpt,sizeof(char));
        if(ret == -1){
            qDebug()<<"setsockopt fail";
            return ;
        }
        NetStudent *stu = new NetStudent();
        connect(s,&QTcpSocket::readyRead,stu,&NetStudent::slotReadyRead);
        stu->setStuId(sId); //分配Id
        sId++;
        stu->setSocket(s);  //关联socket
        stuList.append(stu);        //添加到学生列表
        qDebug()<<"学生 ip:"<<s->peerAddress()<<" port="<<s->peerPort()<<" 已连接RTCP";
    }
}
//发送文件接收命令，告诉对端准备接收文件
bool RTCP::sendFileRcvCommand(int sId,QList<QString> fileList,QList<qint64> fileSizeList)
{
    NetStudent *stu = findStudentById(sId);
    if(stu == nullptr){
        return false;
    }
    QByteArray data;
    QDataStream dataOut(&data,QIODevice::ReadWrite);
    QTcpSocket *s = stu->getSocket();
    QDataStream out(s);
    out.setVersion(QDataStream::Qt_5_1);
    out << QString("fileRcv");//命令字段
    dataOut<<fileList.size();       //待传输文件个数
    for(int i=0;i<fileList.size();i++){
        dataOut << fileList.at(i);  //文件名
        dataOut<< fileSizeList.at(i);   //文件大小
    }
    out << data;
    return true;
}
bool RTCP::sendFileData(int sId,QByteArray data){
    NetStudent *stu = findStudentById(sId);
    if(stu == nullptr){
        return false;
    }
    QTcpSocket *s = stu->getSocket();

    QDataStream out(s);
    out.setVersion(QDataStream::Qt_5_1);
    //out.startTransaction();
    out << QString("fileData");//命令字段
    out << data;
    out.device()->waitForBytesWritten(3000);
    /*if(!out.commitTransaction()){
        qDebug()<<"commitTransaction fail";
        return false;
    }*/
    return true;
}
NetStudent *RTCP::findStudentById(int stuId)
{
    for(int i=0;i<stuList.size();i++){
        if(stuList.at(i)->getStuId() == stuId){
            return stuList.at(i);
        }
    }
    return nullptr;
}
