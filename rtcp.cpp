#include "rtcp.h"
#include<QDataStream>
#include<QTcpServer>
#include<WinSock2.h>
#include<QHostInfo>
#include"networkutils.h"

RTCP * RTCP::pInst = nullptr;

//远程教学控制协议初始化
void RTCP::init(QHostAddress ipAddr,quint16 port)
{
    //服务器创建
    rtcpSrv = new QTcpServer;
    connect(rtcpSrv,&QTcpServer::newConnection,this,&RTCP::newConn);
    //服务器开始监听
    if(!rtcpSrv->listen(ipAddr,port)){
        qDebug()<<"rtcp服务器监听失败！";
        return;
    }
    qDebug()<<"主机名："<<QHostInfo::localHostName();

}
RTCP::RTCP(QHostAddress ipAddr,quint16 port)
{
    init(ipAddr,port);
}

bool RTCP::start(QHostAddress ipAddr,quint16 port){
    if(pInst == nullptr){
        pInst = new RTCP(ipAddr,port);
    }
    if(pInst->rtcpSrv->isListening()){
        qDebug()<<"RTCP 启动成功，端口号="<<port<<" 监听ip="<<ipAddr;
        return true;
    }
    else{
        delete pInst;
        pInst = nullptr;
        qDebug()<<"RTCP 启动失败，请尝试修改RTCP协议端口号 "
                <<"端口号="<<port<<" 监听ip="<<ipAddr;
        return false;
    }
}
RTCP *RTCP::getInstance()
{
    return pInst;
}
void RTCP::newConn(){
    static int sId = 1;
    if(rtcpSrv->hasPendingConnections()){
        QTcpSocket *s = rtcpSrv->nextPendingConnection();
        NetStudent *stu = new NetStudent();
        stu->setStuId(sId); //分配Id
        sId++;
        stu->setSocket(s);  //关联socket
        connect(s,&QTcpSocket::readyRead,stu,&NetStudent::slotReadyRead);
        connect(s,&QTcpSocket::errorOccurred,stu,&NetStudent::slotErrorOccurred);
        connect(stu,&NetStudent::sigDisconnected,this,&RTCP::sigStuDisconnected);
        connect(stu,&NetStudent::sigNewMessage,this,&RTCP::sigNewMessage);
        /*SOCKET nativeSkt = s->socketDescriptor();
        char chOpt = 1;
        int ret = setsockopt(nativeSkt,IPPROTO_TCP,TCP_NODELAY,&chOpt,sizeof(char));
        if(ret == -1){
            qDebug()<<"setsockopt fail";
            return ;
        }*/
        char macAddr[6]={0};
        if(!NetworkUtils::getMacAddrByIp(macAddr,s->peerAddress())){
            qDebug()<<"获取学生端mac地址失败";
        }
        stu->setMacAddr(macAddr);
        stuList.append(stu);        //添加到学生列表
        //发出新学生连接信号
        newStudentConnection(stu->getStuId(),s->peerAddress(),s->peerPort(),macAddr);

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
    if(s->state() != QAbstractSocket::ConnectedState){
        return false;
    }
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


bool RTCP::sendFileRcvCommand(QList<QString> fileList, QList<qint64> fileSizeList)
{
    bool b = true;
    for(int i=0;i<stuList.size();i++){
        if(!sendFileRcvCommand(stuList.at(i)->getStuId(),fileList,fileSizeList)){
            b = false;
        }
    }

    return b;
}

bool RTCP::sendFileData(QByteArray data)
{
    bool b = true;
    for(int i=0;i<stuList.size();i++){
        if(!sendFileData(stuList.at(i)->getStuId(),data)){
            b = false;
        }
    }
    return b;
}

bool RTCP::sendMessage(QByteArray msg)
{
    bool b = true;
    for(int i=0;i<stuList.size();i++){
        if(!sendMessage(stuList.at(i)->getStuId(),msg)){
            b = false;
        }
    }
    return b;
}


bool RTCP::sendFileData(int sId,QByteArray data){
    NetStudent *stu = findStudentById(sId);
    if(stu == nullptr){
        return false;
    }
    QTcpSocket *s = stu->getSocket();
    if(s->state() != QAbstractSocket::ConnectedState){
        return false;
    }
    QDataStream out(s);
    out.setVersion(QDataStream::Qt_5_1);
    out << QString("fileData");//命令字段
    out << data;
    out.device()->waitForBytesWritten(3000);
    return true;
}

//发送网络消息
bool RTCP::sendMessage(int sId,QByteArray msg){
    NetStudent *stu = findStudentById(sId);
    if(stu == nullptr){
        return false;
    }
    QTcpSocket *s = stu->getSocket();
    if(s->state() != QAbstractSocket::ConnectedState){
        return false;
    }
    QDataStream out(s);
    out.setVersion(QDataStream::Qt_5_1);
    out << QString("msg");//命令字段
    out << msg;
    out.device()->waitForBytesWritten(3000);
    return true;
}

//发送关机命令
bool RTCP::sendShutdownCommand(int sId,bool bRestart){
    NetStudent *stu = findStudentById(sId);
    if(stu == nullptr){
        return false;
    }
    QTcpSocket *s = stu->getSocket();
    if(s->state() != QAbstractSocket::ConnectedState){
        return false;
    }
    QDataStream out(s);
    out.setVersion(QDataStream::Qt_5_1);
    QString cmd;
    if(bRestart){
        cmd = "restart";
    }else{
        cmd = "shutdown";
    }
    out << cmd;//命令字段
    QByteArray payload;
    out<<payload;   //空数据体
    out.device()->waitForBytesWritten(3000);
    return true;
}
//发送关机命令给所有学生
bool RTCP::sendShutdownCommand(bool bRestart){
    bool b = true;
    for(int i=0;i<stuList.size();i++){
        if(!sendShutdownCommand(stuList.at(i)->getStuId(),bRestart)){
            b = false;
        }
    }
    return b;
}



//发送开始屏幕共享命令
bool RTCP::sendStartcreenShareCommand(int sId){
    NetStudent *stu = findStudentById(sId);
    if(stu == nullptr){
        return false;
    }
    QTcpSocket *s = stu->getSocket();
    if(s->state() != QAbstractSocket::ConnectedState){
        return false;
    }
    QDataStream out(s);
    out.setVersion(QDataStream::Qt_5_1);
    out << QString("startScreenShare");//命令字段
    QByteArray payload;
    out<<payload;   //空数据体
    out.device()->waitForBytesWritten(3000);
    return true;
}

bool RTCP::sendStartcreenShareCommand(){
    bool b = true;
    for(int i=0;i<stuList.size();i++){
        if(!sendStartcreenShareCommand(stuList.at(i)->getStuId())){
            b = false;
        }
    }
    return b;
}

//发送结束屏幕共享命令
bool RTCP::sendStopScreenShareCommand(int sId){
    NetStudent *stu = findStudentById(sId);
    if(stu == nullptr){
        return false;
    }
    QTcpSocket *s = stu->getSocket();
    if(s->state() != QAbstractSocket::ConnectedState){
        return false;
    }
    QDataStream out(s);
    out.setVersion(QDataStream::Qt_5_1);
    out << QString("stopScreenShare");//命令字段
    QByteArray payload;
    out<<payload;   //空数据体
    out.device()->waitForBytesWritten(3000);
    return true;
}

bool RTCP::sendStopScreenShareCommand(){
    bool b = true;
    for(int i=0;i<stuList.size();i++){
        if(!sendStopScreenShareCommand(stuList.at(i)->getStuId())){
            b = false;
        }
    }
    return b;
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
