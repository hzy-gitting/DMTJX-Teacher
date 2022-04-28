#include "netstudent.h"
#include"filereceiver.h"

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

NetStudent::NetStudent(QObject *parent):QObject(parent)
{
    stuId = -2;
    socket = nullptr;
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
    if(socket->bytesAvailable()){
        slotReadyRead();
    }
}
