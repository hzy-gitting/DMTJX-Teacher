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
#include"teachersndmsgwindow.h"
#include<QMessageBox>
#include<QTableWidgetItem>
#include"MonitorMaster.h"
#include"ffmpeg.h"
#include"rgb2yuv.h"
#include"ffmpegEncoder.h"
#include<iphlpapi.h>
#include"sendfiledialog.h"
#include<QProgressBar>
#include"networkutils.h"
#include"rtcp.h"
#include<QMenuBar>
#include<QMenu>
#include<QAction>
#include<QIcon>
#include<QPushButton>
#include <QThread>
#include"parametersetttingdialog.h"
#include "systemconfigurationinfo.h"
#include"sendscreenvideodatathread.h"
#include"networkmessagelist.h"
#include"netmessage.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    qDebug()<<QThread::currentThread();


    //初始化菜单
    QMenuBar *menuBar =  new QMenuBar(this);
    QMenu *functionMenu = menuBar->addMenu("功能菜单");
    QAction *paramSetupAction = functionMenu->addAction("参数设置");
    connect(paramSetupAction,&QAction::triggered,this,&Widget::setUpParam);
    //QAction *infoAction = setupMenu->addAction("主机信息");
    //connect


    //初始化学生列表
    bool sucConn = (bool)connect(RTCP::getInstance(),&RTCP::newStudentConnection,
            this,&Widget::studentTableAddItem);
    if(!sucConn){
        QMessageBox::information(this,"tips","connect fail");
        qDebug()<<"connect fail";
    }

    connect(RTCP::getInstance(),&RTCP::sigStuDisconnected,
                this,&Widget::slotStuDisconnected);
    nm = NetworkMessageList::getInstance();
    connect(nm,&NetworkMessageList::newMessage,this,&Widget::slotNewMessage);


    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("学生ID"));
    ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("IP"));
    ui->tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("端口号"));
    ui->tableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("MAC地址"));

    QListWidget *stuLstWidget = ui->studentListWidget;

    //stuLstWidget->setGeometry(900,100,300,300);
    stuLstWidget->setIconSize(QSize(100,150));
    //ui->listWidget->setGridSize(QSize(100,100));
    stuLstWidget->setMovement(QListView::Snap);
    stuLstWidget->setResizeMode(QListView::Adjust);
    stuLstWidget->setSpacing(20);
    QListWidgetItem *imageItem = new QListWidgetItem;
    imageItem->setIcon(QIcon("E:/g.png"));
    imageItem->setText("小明");
    imageItem->setToolTip("ip:1.1.3.4");
    stuLstWidget->addItem(imageItem);
    imageItem = new QListWidgetItem;
    imageItem->setIcon(QIcon("E:/g.png"));
    imageItem->setText("小红");
    stuLstWidget->addItem(imageItem);

    //教师端一定要绑定到连接教室局域网的网络接口，不然发送广播包时系统可能使用其他的接口，这样就不对了
    //学生的ip在回应包头就可以得到
    //查学生的mac地址，用到win32 API 获取arp表项 4-15
    userv=new QUdpSocket();
    QHostAddress ip1 = SystemInfo::getSystemInfo()->getIpAddr();
    userv->bind(ip1,8891);

    connect(userv,&QUdpSocket::readyRead,this,&Widget::uservRDRD);
    static QTimer dtimer;
    connect(&dtimer,&QTimer::timeout,this,&Widget::stuDetect);
    dtimer.start(3000);
    //学生探测
    stuDetect();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::slotNewMessage(){
    qDebug()<<__FUNCTION__;
    QList< NetMessage> msgList =  nm->getAllMessage();
    qDebug()<<"getAllMessage";
    QString ss;
    NetMessage msg;
    for(int i=0;i<msgList.size();i++){
        const NetMessage &msg = msgList.at(i);
        ss+="学生(";
        ss+=msg.getSenderAddr().toString();
        ss+=") ";
        ss+=msg.getDateTime().toString("yyyy-MM-dd HH:mm:ss");
        ss+="\n    ";
        ss+=msg.getContent();
        ss+="\n";
    }
    ui->msgRcvEdit->setPlainText(ss);
    ui->msgRcvEdit->moveCursor(QTextCursor::End);
    qDebug()<<__FUNCTION__ <<" end";
}

void Widget::studentTableAddItem(int sId,QHostAddress ip,qint32 port,char macAddr[]){

    QString sMac = NetworkUtils::macToString(macAddr);
    qDebug()<<"新学生连接到RTCP id="<<sId<<" ip="<<ip<<" port="
        <<port<<" MAC地址 "<<sMac;
    QTableWidget *tab = ui->tableWidget;
    int rowIndex = tab->rowCount();
    tab->setRowCount(rowIndex + 1); //增加一行
    tab->setItem(rowIndex,0,new QTableWidgetItem(QString::number(sId)));    //学生id
    tab->setItem(rowIndex,1,new QTableWidgetItem(ip.toString()));    //学生ip
    tab->setItem(rowIndex,2,new QTableWidgetItem(QString::number(port)));    //学生端口
    tab->setItem(rowIndex,3,new QTableWidgetItem(sMac));    //学生mac地址

    QListWidget *stuLstWidget = ui->studentListWidget;
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setIcon(QIcon("E:/g.png"));
    newItem->setText(ip.toString());
    newItem->setData(Qt::UserRole,sId);
    QByteArray macBA(macAddr,6);
    qDebug()<<macBA.size();
    newItem->setData(Qt::UserRole+1,macBA);
    newItem->setData(Qt::UserRole+2,ip.toString());
    QString toolTip = "ID:"+QString::number(sId)+
            "\nIP地址："+ip.toString()+
            "\n端口号："+QString::number(port)+
            "\nMAC地址："+sMac;
    newItem->setToolTip(toolTip);
    stuLstWidget->addItem(newItem);

}

//学生掉线处理
void Widget::slotStuDisconnected(int sId){
    qDebug()<<"学生 id="<<sId<<" 掉线了。";

}
//参数设置
void Widget::setUpParam(){
    qInfo()<<"进行参数设置";
    ParameterSetttingDialog dlg;
    dlg.exec();
}

//探测端口接收到数据
void Widget::uservRDRD(){
    int len = 50000;
    char *data = new char[len];
    int n=0;
    n = userv->bytesAvailable();
    QHostAddress addr;
    quint16 port;
    if(n){
        if(userv->readDatagram(data,n,&addr,&port)){
            data[n]='\0';
            qDebug()<<data;
            if(strcmp(data,"detectReply") == 0){
                qDebug()<<"detect student ip:"<<addr<<"port:"<<port;
            }
        }
    }
    delete[] data;
}



//tcp发送
void Widget::on_tcpSendBtn_clicked()
{
    qInfo()<<"打开文件发送窗口";
    SendFileDialog sfd;
    sfd.exec();
}


//绘图事件
void Widget::paintEvent(QPaintEvent *event)
{
   /* QImage img;
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
    painter.drawLine(0,0,100,100);*/

}


//远程关机
void Widget::on_pushButton_3_clicked()
{
    qInfo()<<"点击远程关机按钮";
    RTCP *rtcp = RTCP::getInstance();
    QListWidget *lw = ui->studentListWidget;
    QList<QListWidgetItem*> selItems = lw->selectedItems();
    QListWidgetItem *item;
    if(selItems.size()<=0){
        QMessageBox::information(this,"提示","请至少选择一个学生机来关机");
        return;
    }
    QString sIp;
    for(int i=0;i<selItems.size();i++){
        item=selItems.at(i);
        sIp = item->data(Qt::UserRole+2).toString();
        qDebug()<<"ip="<<sIp;
        if(!rtcp->sendShutdownCommand(stuIDSelectedList.at(i))){
            QMessageBox::information(this,"提示",
                      "学生id="+QString::number(i)+"ip="+sIp+"发送远程关机命令失败");
        }
    }
    QMessageBox::information(this,"提示","任务完成");
}

//远程重启按钮
void Widget::on_remoteRestartBtn_clicked()
{
    qInfo()<<"点击远程重启按钮";
    RTCP *rtcp = RTCP::getInstance();
    QListWidget *lw = ui->studentListWidget;
    QList<QListWidgetItem*> selItems = lw->selectedItems();
    QListWidgetItem *item;
    if(selItems.size()<=0){
        QMessageBox::information(this,"提示","请至少选择一个学生机来关机");
        return;
    }
    QString sIp;
    for(int i=0;i<selItems.size();i++){
        item=selItems.at(i);
        sIp = item->data(Qt::UserRole+2).toString();
        qDebug()<<"ip="<<sIp;
        if(!rtcp->sendShutdownCommand(stuIDSelectedList.at(i),true)){
            QMessageBox::information(this,"提示",
                      "学生id="+QString::number(i)+"ip="+sIp+"发送远程重启命令失败");
        }
    }
    QMessageBox::information(this,"提示","任务完成");
}

//网络发现：探测在线的学生客户端，返回在线的学生ip、mac地址
void Widget::stuDetect(){
    //发送探测包
    QNetworkDatagram *dg = new QNetworkDatagram();//udp最大发送报文长度大概在65500左右
    QByteArray ba;
    QDataStream out(&ba,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_1);
    out<<QString("detect");
    quint16 rtcpPort = SystemInfo::getSettings()->value("RTCP_port").toUInt();
    out<<rtcpPort;
    dg->setData(ba);
    dg->setDestination(QHostAddress::Broadcast,8900);//探测端口默认8900，ip为受限广播地址，只探测本网段所有主机
    if(-1 == userv->writeDatagram(*dg)){
        qDebug()<<"detect fail:"<<userv->error();
    }
    delete dg;

}
//学生探测按钮
void Widget::on_pushButton_4_clicked()
{
    stuDetect();
}

//“网络对话”按钮
void Widget::on_pushButton_5_clicked()
{
    qInfo()<<"打开网络对话窗口";
    TeacherSndMsgWindow *tsw = new TeacherSndMsgWindow(nullptr);
    //dlg->resize(400,200);
    tsw->show();
}


//开始屏幕共享
void Widget::on_screenShareBtn_clicked()
{

    static bool bStarted = false;
    static QThread *thread;
    static SendScreenVideoDataThread *wt;
    QPushButton *btn = ui->screenShareBtn;
    btn->setEnabled(false);     //先禁用按钮
    if(!bStarted){
        qInfo()<<"开始屏幕共享";
        thread = new QThread();
        wt = new SendScreenVideoDataThread;
        wt->moveToThread(thread);
        connect(this,&Widget::startScreenShare,wt,&SendScreenVideoDataThread::start);
        connect(thread,&QThread::finished,this,&Widget::tfinish);

        //connect(this,&Widget::stopScreenShare,thread,&QThread::quit);
        thread->start();
        emit startScreenShare();

        RTCP *rtcp = RTCP::getInstance();
        if(!rtcp->sendStartcreenShareCommand()){
            QMessageBox::information(this,"提示","开始屏幕共享命令部分发送失败");
        }

        bStarted = true;
        btn->setText("结束屏幕共享");
    }else{
        qInfo()<<"结束屏幕共享";
        thread->requestInterruption();
        //emit stopScreenShare();
        thread->quit();
        thread->wait();
        delete thread;
        delete wt;
        RTCP *rtcp = RTCP::getInstance();
        if(!rtcp->sendStopScreenShareCommand()){
            QMessageBox::information(this,"提示","结束屏幕共享命令部分发送失败");
        }
        btn->setText("开始屏幕共享");
        bStarted = false;
    }
    btn->setEnabled(true);      //启用按钮
}
void Widget::tfinish(){
    qDebug()<<__FUNCTION__;
}


//查看学生提交的作业（打开文件接收目录）
void Widget::on_openFileRcvDirBtn_clicked()
{
    qInfo()<<"查看学生提交的作业";
    QString dir = SystemInfo::getSettings()->value("fileRcvPath").toString();
    ShellExecuteA(NULL,"open",NULL,NULL,dir.toLocal8Bit().data(),SW_SHOWNORMAL);
}

//远程开机
void Widget::on_remoteWakeBtn_clicked()
{
    //发送magic pakcet
    /*幻数据包是一个广播帧，包含目标计算机的MAC地址。
     * 由于 MAC 地址的唯一性，使数据包可以在网络中被唯一的识别。
     * 幻数据包发送通常使用无连接的传输协议，如 UDP ，
     * 发送端口为 7 或 9 ，这只是通常做法，没有限制。
    幻数据包最简单的构成是6字节的255（FF FF FF FF FF FF FF），
    紧接着为目标计算机的48位MAC地址，重复16次，数据包共计102字节。
    有时数据包内还会紧接着4-6字节的密码信息。这个帧片段可以包含在任何协议中
    ，最常见的是包含在 UDP 中。*/

    qInfo()<<"点击远程开机按钮";
    QListWidget *lw = ui->studentListWidget;
    QList<QListWidgetItem*> selItems = lw->selectedItems();
    QListWidgetItem *item;
    if(selItems.size()<=0){
        QMessageBox::information(this,"提示","请至少选择一个学生机来唤醒");
        return;
    }
    char *macAddr;
    for(int i=0;i<selItems.size();i++){
        item=selItems.at(i);
        QByteArray ba = item->data(Qt::UserRole+1).toByteArray();
        for(int i=0;i<6;i++){
            qDebug()<<(unsigned int)(unsigned char)(ba.at(i));
        }
        macAddr = ba.data();
        for(int i=0;i<6;i++){
            qDebug()<<QString::number((unsigned int)(unsigned char)macAddr[i],16);
        }
        QString sMac = NetworkUtils::macToString(macAddr);
        qDebug()<<"macAddr="<<sMac;
        if(!NetworkUtils::sendMagicPacket(userv,macAddr)){
            qDebug()<<"sendMagicPacket failed";
            QMessageBox::information(this,"提示",
                                     "发送"+sMac+"的魔法包失败");
            return;
        }
    }
    QMessageBox::information(this,"提示","任务完毕");
}



void Widget::on_studentListWidget_itemSelectionChanged()
{
    qInfo()<<"更改学生选择";
    QListWidget *lw = ui->studentListWidget;
    QList<QListWidgetItem*> selItems = lw->selectedItems();
    QListWidgetItem *item;
    stuIDSelectedList.clear();
    qDebug()<<"当前选中"<<selItems.size()<<"个学生";
    for(int i=0;i<selItems.size();i++){
        item=selItems.at(i);
        int id;
        id = item->data(Qt::UserRole).toInt();
        qDebug()<<item->data(Qt::UserRole+1);
        qDebug()<<"sId="<<id;
        stuIDSelectedList.append(id);
    }
}


