#include "sendfiledialog.h"
#include "ui_sendfiledialog.h"

#include <QFileDialog>
#include<QFile>
#include<QTcpServer>
#include<iterator>
#include<QTcpSocket>
#include<QMessageBox>
#include"rtcp.h"


SendFileDialog::SendFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SendFileDialog)
{
    ui->setupUi(this);

    //文件发送列表 设置表头
    ui->fileTable->setColumnCount(2);
    //ui->fileTable->setRowCount(2);
    ui->fileTable->setHorizontalHeaderItem(0,new QTableWidgetItem("文件名"));
    ui->fileTable->setHorizontalHeaderItem(1,new QTableWidgetItem("大小"));

}


SendFileDialog::~SendFileDialog()
{
    delete ui;
}
//选择文件
void SendFileDialog::on_chooseFileBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"选择文件");
    if(fileName.isEmpty()){
        return;
    }
    int rowIndex = ui->fileTable->rowCount();
    ui->fileTable->setRowCount(rowIndex+1);//添加一行
    ui->fileTable->setItem(rowIndex,0,new QTableWidgetItem(fileName));

    QFile f(fileName);

    ui->fileTable->setItem(rowIndex,1,new QTableWidgetItem(QString::number(f.size(),10)));
    fileList.append(fileName);
    fileSizeList.append(f.size());
}

//发送文件
void SendFileDialog::on_sendBtn_clicked()
{
    //禁用该按钮
    ui->sendBtn->setDisabled(true);

    //创建文件传输服务器
    /*fsrv = new QTcpServer;
    if(!fsrv->listen(QHostAddress::Any,8979)){
        qDebug()<<"listen fail";
    }
    connect(fsrv,&QTcpServer::newConnection,this,&SendFileDialog::newConn);*/
    //通知学端连接教师，准备接收文件，告知文件名称、大小、服务端口
    //调用RTCP
    RTCP *rtcp = RTCP::getInstance();
    if(!rtcp->sendFileRcvCommand(fileList,fileSizeList)){
        qDebug()<<"部分文件传输命令发送失败";
        return;
    }

    //连接成功后，发送内容
    QFile f;
    QByteArray fdata;   //文件数据
    int blockSize = 1024;   //每次读取的文件块大小
    char *data = new char[blockSize];
    for(int i=0;i<fileList.size();i++){
        f.setFileName(fileList.at(i));
        if(!f.open(QIODevice::ReadOnly)){
            qDebug()<<"fopen fail";
            QMessageBox::information(this,"提示","打开文件"+f.fileName()+"失败，请重试");
            delete[]data;
            return;
        }
        while(f.bytesAvailable()){
            qint64 n = f.read(data,blockSize);
            if(-1 == n){
                qDebug()<<"read file fail";
                QMessageBox::information(this,"提示","读取文件"+f.fileName()+"失败，请重试");
                delete[]data;
                return;
            }
            qDebug()<<"read file n="<<n;
            fdata.setRawData(data,n);
            if(!rtcp->sendFileData(fdata)){
                qDebug()<<"部分学生发送失败";
                return;
            }
            QGuiApplication::processEvents();
        }
        f.close();
    }

    QMessageBox::information(this,"提示","发送完成");

    delete[]data;
    ui->sendBtn->setEnabled(true);
}

//新连接到来
void SendFileDialog::newConn(){
    if(fsrv->hasPendingConnections()){
        QTcpSocket *s = fsrv->nextPendingConnection();
        \
        socketList.append(s);   //添加到套接字列表中
        qDebug()<<"第"<<socketList.size()<<"个连接："<<s->peerAddress()<<" port="<<s->peerPort();
        if(socketList.size()>=nStudent){
            startSendFileData();
        }
    }
}

//正式发送文件内容
void SendFileDialog::startSendFileData(){
    QListIterator it(socketList);
    QTcpSocket *s;
    QFile f;
    QByteArray fdata;   //文件数据
    for(int i=0;i<fileList.size();i++){
        f.setFileName(fileList.at(i));
        if(!f.open(QIODevice::ReadOnly)){
            qDebug()<<"fopen fail";
            return;
        }
        fdata.append(f.readAll());
    }

    while(it.hasNext()){
        s = it.next();
        if(-1 == s->write(fdata)){
            qDebug()<<"发送文件时发生错误："<<s->errorString();
            return;
        }
    }
}

void SendFileDialog::on_closeBtn_clicked()
{
    close();
}

