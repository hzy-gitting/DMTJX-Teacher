#include "netstudent.h"
#include"filereceiver.h"
#include<QHostAddress>

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

void NetStudent::setMacAddr(const char mac[])
{
    for(int i=0;i<6;i++){
        macAddr[i]=mac[i];
    }
}

void NetStudent::getMacAddr(char mac[])
{
    for(int i=0;i<6;i++){
        mac[i]=macAddr[i];
    }
}

const QString &NetStudent::getComputerName() const
{
    return computerName;
}

void NetStudent::setComputerName(const QString &newComputerName)
{
    computerName = newComputerName;
}

NetStudent::NetStudent(QObject *parent) : QObject(parent)
{
    stuId = -2;
    socket = nullptr;
    isOnLine = true;
}

//处理来自学生端的数据
void NetStudent::slotReadyRead(){
    qDebug()<<"收到来自学生id="<<stuId<< " 的数据";
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_1);
    QString cmd;
    QByteArray payload;
    in.startTransaction();
    in >>cmd;   //读取命令字段
    in >> payload;  //读取数据体（负载）
    qDebug()<<"cmd="<<cmd;
    if(!in.commitTransaction()){
        qDebug()<<"commitTransaction fail";
        return;
    }
    if(cmd == "fileRcv"){//准备接收文件
        QDataStream inData(payload);
        QList<QString> fileNameList;
        QList<qint64> fileSizeList;
        inData >> fileNameList;
        inData >> fileSizeList;
        qDebug()<<"准备接收文件 数量="<<fileNameList.size();

        for(int i=0;i<fileNameList.size();i++){
            qDebug()<<"文件名："<<fileNameList.at(i)
                   <<" 大小="<<fileSizeList.at(i);
        }
        frcver = new FileReceiver(fileNameList,fileSizeList);
    }
    else if(cmd == "fileData"){//文件数据
        qDebug()<<"收到文件数据";
        frcver->writeFile(payload);
    }
    else if(cmd == "msg"){
        qDebug()<<"收到网络消息";
        //将消息信号传递给RTCP
        emit sigNewMessage(payload,socket->peerAddress());
    }
    if(socket->bytesAvailable()){
        slotReadyRead();
    }
}

//处理来自学生端套接字错误处理
void NetStudent::slotErrorOccurred(QAbstractSocket::SocketError socketError){
    qDebug()<<__FUNCTION__<<" "<<socketError;
    qDebug()<<socket->errorString();
    if(socketError == QAbstractSocket::RemoteHostClosedError){
        isOnLine=false;
        qDebug()<<"学生 id="<<stuId<<"ip="<<socket->peerAddress().toString()
               <<" 连接已断开";
        emit sigDisconnected(stuId);
    }
}
