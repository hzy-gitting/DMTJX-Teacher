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
#include"parametersetttingdialog.h"
#include "systemconfigurationinfo.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
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
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("学生ID"));
    ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("IP"));
    ui->tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("端口号"));
    ui->tableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("MAC地址"));

    ui->listWidget->setGeometry(900,100,300,300);
    ui->listWidget->setIconSize(QSize(100,150));
    //ui->listWidget->setGridSize(QSize(100,100));
    ui->listWidget->setMovement(QListView::Snap);
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setSpacing(20);
    QListWidgetItem *imageItem = new QListWidgetItem;
    imageItem->setIcon(QIcon("E:/g.jpg"));
    imageItem->setText("gg1");
    imageItem->setToolTip("ip:1.1.3.4");
    ui->listWidget->addItem(imageItem);
    imageItem = new QListWidgetItem;
    imageItem->setIcon(QIcon("E:/g.jpg"));
    imageItem->setText("gg2");
    ui->listWidget->addItem(imageItem);
    imageItem = new QListWidgetItem;
    imageItem->setIcon(QIcon("E:/g.jpg"));
    imageItem->setText("gg3");
    ui->listWidget->addItem(imageItem);
    imageItem = new QListWidgetItem;
    imageItem->setIcon(QIcon("E:/g.jpg"));
    imageItem->setText("gg4");
    ui->listWidget->addItem(imageItem);
    imageItem = new QListWidgetItem;
    imageItem->setIcon(QIcon("E:/g.jpg"));
    imageItem->setText("gg5");
    ui->listWidget->addItem(imageItem);

    imageItem = new QListWidgetItem;
    imageItem->setIcon(QIcon("E:/g.jpg"));
    imageItem->setText("gg6");
    ui->listWidget->addItem(imageItem);

    imageItem = new QListWidgetItem;
    imageItem->setIcon(QIcon("E:/g.jpg"));
    imageItem->setText("gg7");
    ui->listWidget->addItem(imageItem);

    imageItem = new QListWidgetItem;
    imageItem->setIcon(QIcon("E:/g.jpg"));
    imageItem->setText("gg8");
    ui->listWidget->addItem(imageItem);

    imageItem = new QListWidgetItem;
    imageItem->setIcon(QIcon("E:/g.jpg"));
    imageItem->setText("gg9");
    ui->listWidget->addItem(imageItem);

    //教师端一定要绑定到连接教室局域网的网络接口，不然发送广播包时系统可能使用其他的接口，这样就不对了
    //学生的ip在回应包头就可以得到
    //查学生的mac地址，用到win32 API 获取arp表项 4-15
    userv=new QUdpSocket();
    QHostAddress ip1 = SystemInfo::getSystemInfo()->getIpAddr();
    userv->bind(ip1,8891);

    connect(userv,&QUdpSocket::readyRead,this,&Widget::uservRDRD);



    controlSocket = new QTcpSocket();
    controlSocket->connectToHost(QHostAddress::LocalHost,8888);//连接到学生


    //学生探测
    stuDetect();
}

Widget::~Widget()
{
    delete ui;
}
void Widget::studentTableAddItem(int sId,QHostAddress ip,qint32 port,char macAddr[]){
    qDebug()<<"新学生连接到RTCP id="<<sId<<" ip="<<ip<<" port="
        <<port<<" MAC地址 "<<NetworkUtils::macToString(macAddr);
    QTableWidget *tab = ui->tableWidget;
    int rowIndex = tab->rowCount();
    tab->setRowCount(rowIndex + 1); //增加一行
    tab->setItem(rowIndex,0,new QTableWidgetItem(QString::number(sId)));    //学生id
    tab->setItem(rowIndex,1,new QTableWidgetItem(ip.toString()));    //学生ip
    tab->setItem(rowIndex,2,new QTableWidgetItem(QString::number(port)));    //学生端口
    tab->setItem(rowIndex,3,new QTableWidgetItem(NetworkUtils::macToString(macAddr)));    //学生mac地址
}
//参数设置
void Widget::setUpParam(){
    qDebug()<<"进行参数设置";
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
    controlSocket->write("shutdown");
}

//网络发现：探测在线的学生客户端，返回在线的学生ip、mac地址
void Widget::stuDetect(){
    //1发送探测包
    //QUdpSocket *udp = new QUdpSocket();
    QNetworkDatagram *dg = new QNetworkDatagram();//udp最大发送报文长度大概在65500左右
    dg->setData(QByteArray("detect"));
    //dg->setSender(QHostAddress("192.168.173.1"),12345);

    dg->setDestination(QHostAddress::Broadcast,8900);//探测端口默认8900，ip为受限广播地址，只探测本网段所有主机
    if(-1 == userv->writeDatagram(*dg)){
        qDebug()<<"detect fail:"<<userv->error();
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
    TeacherSndMsgWindow *tsw = new TeacherSndMsgWindow(this);
    //dlg->resize(400,200);
    tsw->show();
}

struct cap_screen_t
{
    HDC memdc;
    HBITMAP hbmp;
    unsigned char* buffer;
    int            length;

    int width;
    int height;
    int bitcount;
    int left, top;
};

extern MonitorMaster::VEC_MONITOR_INFO vecMonitorListInfo;

int init_cap_screen(struct cap_screen_t* sc, int indexOfMonitor = 0)
{
    MonitorMaster::GetAllMonitorInfo();
    if (indexOfMonitor >= vecMonitorListInfo.size())
        indexOfMonitor = 0;
    DEVMODE devmode;
    BOOL bRet;
    BITMAPINFOHEADER bi;
    sc->width = vecMonitorListInfo[indexOfMonitor]->nWidth;
    sc->height = vecMonitorListInfo[indexOfMonitor]->nHeight;
    sc->bitcount = vecMonitorListInfo[indexOfMonitor]->nBits;
    sc->left = vecMonitorListInfo[indexOfMonitor]->area.left;
    sc->top = vecMonitorListInfo[indexOfMonitor]->area.top;
    memset(&bi, 0, sizeof(bi));
    bi.biSize = sizeof(bi);
    bi.biWidth = sc->width;
    bi.biHeight = -sc->height; //从上朝下扫描
    bi.biPlanes = 1;
    bi.biBitCount = sc->bitcount; //RGB
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    HDC hdc = GetDC(NULL); //屏幕DC
    sc->memdc = CreateCompatibleDC(hdc);
    sc->buffer = NULL;
    sc->hbmp = CreateDIBSection(hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&sc->buffer, NULL, 0);
    ReleaseDC(NULL, hdc);
    SelectObject(sc->memdc, sc->hbmp); ///
    sc->length = sc->height* (((sc->width*sc->bitcount / 8) + 3) / 4 * 4);
    return 0;
}

HCURSOR FetchCursorHandle()
{
    CURSORINFO hCur;
    ZeroMemory(&hCur, sizeof(hCur));
    hCur.cbSize = sizeof(hCur);
    GetCursorInfo(&hCur);
    return hCur.hCursor;
}

void AddCursor(HDC hMemDC, POINT origin)
{

    POINT xPoint;
    GetCursorPos(&xPoint);
    xPoint.x -= origin.x;
    xPoint.y -= origin.y;
    if (xPoint.x < 0 || xPoint.y < 0)
        return;
    HCURSOR hcur = FetchCursorHandle();
    ICONINFO iconinfo;
    BOOL ret;
    ret = GetIconInfo(hcur, &iconinfo);
    if (ret)
    {
        xPoint.x -= iconinfo.xHotspot;
        xPoint.y -= iconinfo.yHotspot;
        if (iconinfo.hbmMask) DeleteObject(iconinfo.hbmMask);
        if (iconinfo.hbmColor) DeleteObject(iconinfo.hbmColor);
    }
    DrawIcon(hMemDC, xPoint.x, xPoint.y, hcur);
}

int blt_cap_screen(struct cap_screen_t* sc)
{
    HDC hdc = GetDC(NULL);
    BitBlt(sc->memdc, 0, 0, sc->width, sc->height, hdc, sc->left, sc->top, SRCCOPY); // 截屏
    AddCursor(sc->memdc, POINT{ sc->left, sc->top }); // 增加鼠标进去
    ReleaseDC(NULL, hdc);
    return 0;
}


//开始屏幕广播
void Widget::on_pushButton_2_clicked()
{
    /*QTimer *timer = new QTimer(this);
    timer->start(10);
    connect(timer,&QTimer::timeout,this,&Widget::on_btnSaveScreen_clicked);*/

    struct cap_screen_t sc;
    BYTE* out;
    AVFrame* frame;
    time_t start, end;

    ffmpeg_init();
    init_cap_screen(&sc, 1);  // 1代表第二块显示器，0代表第一块显示器，依次类推，如果显示器数量不足则为0
    out = (BYTE*)malloc(sc.length);

    // SDLMaster::init(sc.width, sc.height); // SDL播放器，可以用于播放测试
    YUVencoder enc(sc.width, sc.height);

    // 以下为3个不同的编码器，任选一个即可
    x264Encoder ffmpeg264(sc.width, sc.height,  "save.h264"); // save.h264为本地录屏文件 可以使用vlc播放器播放
    //mpeg1Encoder ffmpegmpeg1(sc.width, sc.height, "save.mpg");
    //libx264Master libx264(sc.width, sc.height, "save2.h264");

    time(&start);
    for (int i = 0; ; i++)
    {
        blt_cap_screen(&sc);//20-30ms 我认为可以单独使用一个线程专门来截屏，提高帧率
         qDebug()<<"blt_cap_screen end "<<QTime::currentTime();

        frame = enc.encode(sc.buffer, sc.length, sc.width, sc.height, out, sc.length);	//rgb转yuv 8ms

        qDebug()<<"rgb2yuv end "<<QTime::currentTime();

        if (frame == NULL)
        {
            printf("Encoder error!!\n"); Sleep(1000); continue;
        }

        // 一下是3种编码方式，任选一种均可
        ffmpeg264.encode(frame);	//h.264视频编码，保存到文件       瓶颈2：网络速率 50-500-1000ms  qt的网络封装导致速度慢？

        qDebug()<<"teac enc end "<<QTime::currentTime();

        QApplication::processEvents();

        // ffmpegmpeg1.encode(frame);
        // libx264.encode(frame);

        // SDL播放器播放视频，取消注释即可播放
         //SDLMaster::updateScreen(frame);

    }
    fflush(stdout);
    qDebug()<<".@#$%^&*()_+.";
    time(&end);
    printf("%f\n", difftime(end, start));
    fflush(stdout);
    ffmpeg264.flush(frame);
    //SDL_Quit();
    return ;

}

/*int main(int argc, char* argv[])
{
    struct cap_screen_t sc;
    BYTE* out;
    AVFrame* frame;
    web_stream* web = new web_stream;
    time_t start, end;
    WSADATA d;

    ffmpeg_init();
    init_cap_screen(&sc, 1);  // 1代表第二块显示器，0代表第一块显示器，依次类推，如果显示器数量不足则为0
    out = (BYTE*)malloc(sc.length);

     SDLMaster::init(sc.width, sc.height); // SDL播放器，可以用于播放测试
    YUVencoder enc(sc.width, sc.height);

    // 以下为3个不同的编码器，任选一个即可
    x264Encoder ffmpeg264(sc.width, sc.height,  "save.h264"); // save.h264为本地录屏文件 可以使用vlc播放器播放
    //mpeg1Encoder ffmpegmpeg1(sc.width, sc.height, "save.mpg");
    //libx264Master libx264(sc.width, sc.height, "save2.h264");

    WSAStartup(0x0202, &d);
    web->start("0.0.0.0", 8000); // 8000端口侦听
    time(&start);
    FILE* fp1 = fopen("sc.y", "wb");
    if (!fp1) {
        printf("fopen fail");
        return -1;
    }
    for (int i = 0; i < 200; i++)
    {
        blt_cap_screen(&sc);//20-30ms 我认为可以单独使用一个线程专门来截屏，提高帧率
        char* buf = NULL;
        dp_frame_t a;
        a.bitcount = 32;
        a.buffer = (char*)sc.buffer;
        a.line_bytes = sc.width * sc.bitcount / 8;
        a.line_stride = (a.line_bytes + 3) / 4 * 4;
        a.cx = sc.width;
        a.cy = sc.height;
        web->frame(&a);  // 发送帧

        frame = enc.encode(sc.buffer, sc.length, sc.width, sc.height, out, sc.length);	//rgb转yuv 8ms

        fwrite(frame->data[0], sc.width * sc.height, 1, fp1);
        if (frame == NULL)
        {
            printf("Encoder error!!\n"); Sleep(1000); continue;
        }

        // 一下是3种编码方式，任选一种均可
        ffmpeg264.encode(frame);	//h.264视频编码，保存到文件
        // ffmpegmpeg1.encode(frame);
        // libx264.encode(frame);

        // SDL播放器播放视频，取消注释即可播放
         SDLMaster::updateScreen(frame);

        free(buf);
    }
    fclose(fp1);
    time(&end);
    printf("%f\n", difftime(end, start));
    //ffmpeg264.flush(frame, &sender);
    SDL_Quit();
    return 0;
}
*/
//查看学生提交的作业（打开文件接收目录）
void Widget::on_openFileRcvDirBtn_clicked()
{
    ShellExecuteA(NULL,"open",NULL,NULL,"E:/teacherRcv",SW_SHOWNORMAL);
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

     char macAddr[6];
    if(!NetworkUtils::getMacAddrByIp(macAddr,QHostAddress("192.168.31.1"))){
        qDebug()<<"getMacAddrByIp failed";
        return;
    }
    qDebug()<<"macAddr="<<NetworkUtils::macToString(macAddr);
    if(!NetworkUtils::sendMagicPacket(userv,macAddr)){
        qDebug()<<"sendMagicPacket failed";
        return;
    }
}


