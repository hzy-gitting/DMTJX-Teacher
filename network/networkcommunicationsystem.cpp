#include "networkcommunicationsystem.h"
#include "networkcommunicationsystem.h"
#include<QNetworkInterface>
#include<QTime>
NetworkCommunicationSystem::NetworkCommunicationSystem(QObject *parent) : QObject(parent)
{
    udpSocket = new QUdpSocket();
    QList ifs = QNetworkInterface::allInterfaces();
    for(int i=0;i<ifs.size();i++){
        qDebug()<<ifs[i].name();
        qDebug()<<ifs[i].humanReadableName();
        qDebug()<<ifs[i].type();
        qDebug()<<ifs[i].flags();
        qDebug()<<ifs[i].hardwareAddress();
    }
    qDebug() << QNetworkInterface::allAddresses();
}
//发送tcp数据，返回成功布尔值
bool NetworkCommunicationSystem::tcpSendData(char *data, int size, QHostAddress addr, int port)
{
    //先看是否与该主机建立了tcp连接，如果是，则直接发送数据，否则先连接再发送
    QTcpSocket *s = nullptr;
    int lenWritten = 0;
    if(!isConnectedTo(addr,port)){
        if(!connectTo(addr,port)){
           return false;
        }
    }
    s = getTcpSocket(addr,port);
    while(size > 0){
        QTime start= QTime::currentTime();
        qDebug()<<"start: "<<start;
        lenWritten = s->write(data,size);

        QTime end= QTime::currentTime();
        qDebug()<<"end: "<<end;

        start= QTime::currentTime();
        qDebug()<<"start2: "<<start;
        s->waitForBytesWritten();
        end= QTime::currentTime();
        qDebug()<<"end2: "<<end;

        qDebug()<<"bytestowrite="<<s->bytesToWrite();

        if(lenWritten == -1){
            qDebug()<<s->error();
            emit error();
            return false;
        }
        size -= lenWritten;
        data += lenWritten;
    }
    return true;
}
bool NetworkCommunicationSystem::tcpSendData_block(char *data, int size, QHostAddress addr, int port, int msec)
{
    return false;
}

bool NetworkCommunicationSystem::udpSendData(char *data, int size, QHostAddress addr, int port)
{
    udpSocket->writeDatagram(data,size,addr,port);
    return true;
}

bool NetworkCommunicationSystem::isConnectedTo(QHostAddress addr, int port)
{
    QTcpSocket *s;
    if(tcpSocketMap.contains(addr.toString())){
        s = tcpSocketMap.value(addr.toString());
        if(s->state() == QAbstractSocket::ConnectedState){
            return true;
        }
        else{
            tcpSocketMap.remove(addr.toString());
            s->close();
            delete s;
        }
    }
    return false;
}
//连接
bool NetworkCommunicationSystem::connectTo(QHostAddress addr, int port)
{
    QTcpSocket *s = new QTcpSocket();
    s->connectToHost(addr,port);
    if(s->waitForConnected(5000)){
        connect(s,&QTcpSocket::readyRead,this,&NetworkCommunicationSystem::tcpReadyRead);
        connect(s,&QTcpSocket::bytesWritten,this,&NetworkCommunicationSystem::dataSent);
        tcpSocketMap.insert(addr.toString(),s);
        return true;
    }else{

        qDebug()<<s->error();
        delete s;
        emit error();
        return false;
    }
}

QTcpSocket *NetworkCommunicationSystem::getTcpSocket(QHostAddress addr, int port)
{
    return tcpSocketMap.value(addr.toString());
}

//接收到tcp数据发射dataReceived信号
void NetworkCommunicationSystem::tcpReadyRead(){
    QTcpSocket *s = (QTcpSocket*)sender();
    QByteArray data = s->readAll();
    int port = s->peerPort();
    QHostAddress addr=s->peerAddress();
    emit dataReceived(data,addr,port);
}
