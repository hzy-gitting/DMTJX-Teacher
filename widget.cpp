#include "widget.h"
#include "ui_widget.h"
#include<QNetworkInterface>
#include<QDebug>
#include<QUdpSocket>
#include <QImageReader>
#include <QScreen>
#include <QBuffer>
#include <QTimer>
#include <QNetworkDatagram>
#include"networkcommunicationsystem.h"
#include<QFile>
#include<QFileDialog>
#include<windows.h>
#include<QImage>
#include<QPainter>
#include"messageform.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);


    us=new QUdpSocket();
    us->connectToHost(QHostAddress::LocalHost,8890);
    connect(us,&QUdpSocket::bytesWritten,this,&Widget::written);

     ts=new QTcpSocket();
    connect(ts,&QTcpSocket::connected,this,&Widget::tcpConnected);
    connect(ts,&QTcpSocket::readyRead,this,&Widget::tcpReadyRead);
    connect(ts,&QTcpSocket::errorOccurred,this,&Widget::tcpError);

    connect(&nsys,&NetworkCommunicationSystem::dataSent,this,&Widget::dataSent);

    controlSocket = new QTcpSocket();
    controlSocket->connectToHost(QHostAddress::LocalHost,8888);//连接到学生

    screen = QGuiApplication::primaryScreen();

    //学生探测
    stuDetect();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::written(qint64 bytesWritten){
    qDebug()<<"written" << bytesWritten;
}

void Widget::on_pushButton_clicked()
{
    QString s = ui->lineEdit->text();
    QByteArray ba=s.toLocal8Bit();
    us->connectToHost(QHostAddress(ui->ipEdit->text()),ui->portEdit->text().toInt());
    if(!us->open(QIODevice::ReadOnly)){
        qDebug()<<"soc open fail22";
    }
    if(-1 == us->write(ba)){
        qDebug()<<"udp write fail"<<us->error()<<us->errorString();
    }
    /*if(-1 == us->writeDatagram(ba,ba.length(),QHostAddress(ui->ipEdit->text()),ui->portEdit->text().toInt())){
        qDebug()<<"udp send fail"<<us->errorString();
    }*/
}


void Widget::on_bindButton_clicked()
{
    us->bind(QHostAddress(ui->ipEdit->text()),ui->portEdit->text().toInt());
}

//连接
void Widget::on_connectButton_clicked()
{
    ts->connectToHost(ui->ipEdit->text(),ui->portEdit->text().toInt());
}

//tcp发送
void Widget::on_tcpSendBtn_clicked()
{
    QString msg = ui->textEdit->toPlainText();
    //ts->write(msg.toUtf8());
    QByteArray ba = msg.toUtf8();
    char *data = ba.data();
    int size = ba.size();
    nsys.tcpSendData(data,size,QHostAddress(ui->ipEdit->text()),ui->portEdit->text().toInt());
    qDebug()<<"tcp send";
}
//连接成功
void Widget::tcpConnected(){
    qDebug()<<"连接成功 "<< ts->peerAddress()<<"port="<<ts->peerPort()<<ts->peerName();
}
//接收消息
void Widget::tcpReadyRead(){
    qDebug()<<"tcp read";
    QByteArray b = ts->readAll();
    ui->textEdit->setText(b);
}

//tcp发生错误
void Widget::tcpError(){
    qDebug()<<ts->error();
}

//选择文件
void Widget::on_chooseFileBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"选择文件");
    ui->filePathEdit->setText(fileName);
    ui->filePathEdit->setProperty("sFileName",fileName);
}

//预览文件
void Widget::on_previewFileBtn_clicked()
{
    QString fileName = ui->filePathEdit->property("sFileName").toString();
    QFile f(fileName);
    f.open(QIODevice::ReadOnly);
    QTime start= QTime::currentTime();
    qDebug()<<"start: "<<start;
    QByteArray b = f.read(1024*64);
    QTime end= QTime::currentTime();
    qDebug()<<"end: "<<end;
    qDebug()<<"size="<<b.size()<<"  "<<f.size();
    f.close();qDebug()<<"qs="<<sizeof(qsizetype);qDebug()<<"int="<<sizeof(int);qDebug()<<"int64="<<sizeof(qint64);

//    start= QTime::currentTime();
//    qDebug()<<"start2: "<<start;
//    ui->textEdit->setPlainText(b);
//    end= QTime::currentTime();
//    qDebug()<<"end2: "<<end;
}

//tcp发送文件
void Widget::on_tcpSendFileBtn_clicked()
{
    QString fileName = ui->filePathEdit->text();
    QFile f(fileName);
    if(!f.open(QIODevice::ReadOnly)){
        qDebug()<<"打开文件失败";
        return;
    }
    //发送文件大小
    qint64 fileSize=f.size();
    QByteArray bs;
    QDataStream ds(&bs,QIODevice::WriteOnly);
    ds.setVersion(QDataStream::Qt_5_1);
    ds<<fileSize;
    if(!nsys.tcpSendData(bs.data(),bs.size(),QHostAddress("192.168.1.11"),8887)){
        qDebug()<<"发送数据失败";
        return;
    }

    qint64 bytesRead = 0,bytesLeft = fileSize;
    qint64 const maxBytesPerRead=1024*1024;
    QByteArray b;
    //对于大文件，超过一个G的，读取可能很慢，要分段边读取边发送，提高效率
    while(bytesLeft > 0){
        b = f.read(maxBytesPerRead);
        bytesRead = b.size();
        bytesLeft -= bytesRead;
        if(!nsys.tcpSendData(b.data(),b.size(),QHostAddress("192.168.1.11"),8887)){
            qDebug()<<"发送数据失败";
            return;
        }
    }

    f.close();
}

void Widget::dataSent(qint64 bytesSent){
    qDebug()<<"sent "<<bytesSent<<" bytes";
}
//绘图事件
void Widget::paintEvent(QPaintEvent *event)
{
    QImage img;
    QPainter painter(this);
    QImageReader imgReader = QImageReader("C:/Users/hzy/Desktop/1.bmp");

    if(!imgReader.read(&img)){
        qDebug()<<"img read failed";
    }
    //img.setDevicePixelRatio(1.25);
    //qDebug()<<img.devicePixelRatio();
    painter.setRenderHint(QPainter::LosslessImageRendering);
    painter.drawImage(0,0,img,0,0,0,0);
    //qDebug()<<img.width()<<" "<<img.height()<< " "<<devicePixelRatio()<<" "<<painter.renderHints();
    painter.drawLine(0,0,100,100);
    qDebug()<<"pa";
}
//保存截屏
void Widget::on_btnSaveScreen_clicked()
{
    qDebug()<<"s";

    QPixmap sc = screen->grabWindow(0);//全屏50ms 根据截屏大小而变化，截屏范围越大越耗时  2

    QBuffer buf;
    buf.open(QIODevice::WriteOnly);qDebug()<<QTime::currentTime();
    sc.save(&buf,"jpg",0);    qDebug()<<QTime::currentTime();      //压缩100ms 跟图像压缩质量、大小有关  3
    us->write(buf.buffer());
    us->waitForBytesWritten();
    buf.close();
    qDebug()<<"e";
}

//开始屏幕广播
void Widget::on_pushButton_2_clicked()
{
    QTimer *timer = new QTimer(this);
    timer->start(10);
    connect(timer,&QTimer::timeout,this,&Widget::on_btnSaveScreen_clicked);
}

//远程关机
void Widget::on_pushButton_3_clicked()
{
    controlSocket->write("shutdown");
}

//网络发现：探测在线的学生客户端，返回在线的学生ip、mac地址
void Widget::stuDetect(){
    //1发送探测包
    QUdpSocket *udp = new QUdpSocket();
    QNetworkDatagram *dg = new QNetworkDatagram();
    dg->setData(QByteArray("detect"));
    dg->setDestination(QHostAddress::LocalHost,8900);//探测端口默认8900，广播
    if(-1 == udp->writeDatagram(*dg)){
        qDebug()<<"detect fail";
    }

    //2检测应答包

}
//学生探测按钮
void Widget::on_pushButton_4_clicked()
{
    stuDetect();
}

//“网络对话”按钮
void Widget::on_pushButton_5_clicked()
{
    QWidget *dlg = new MessageForm();
    //dlg->resize(400,200);
    dlg->show();
}

