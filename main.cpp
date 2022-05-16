#include "widget.h"

#include <QApplication>
#include"rtcp.h"
#include<QSettings>
#include"systemconfigurationinfo.h"
#include<QThread>
#include"networkmessagelist.h"
#include <QMutex>

void outputMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    static QMutex mutex;
    mutex.lock();

    bool isDebug = true;
    QFile file;
    QString text;
    switch (type) {

    case QtInfoMsg:
        isDebug = false;
        file.setFileName("操作日志记录.txt");
        break;
    case QtDebugMsg:
        text = QString("debug:");

        break;
    case QtWarningMsg:
        text = QString("warning:");
        break;

    }
    if(isDebug){
        file.setFileName("debug.txt");
    }
    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);

    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.close();

    mutex.unlock();
}


int main(int argc, char *argv[])
{
    //qInstallMessageHandler(outputMessage);
    QApplication a(argc, argv);
    setbuf(stdout, NULL);

    //初始化系统信息
    SystemInfo::initialize();

    QSettings *setting = SystemInfo::getSettings();
    QString macAddr = setting->value("macAddr").toString();

    SystemInfo *sysInfo = SystemInfo::getSystemInfo();

    //读取RTCP端口号
    quint16 rtcp_port = setting->value("RTCP_port").toUInt();
    QHostAddress ip = sysInfo->getIpAddr();

    RTCP::start(ip,rtcp_port);  //启动远程教学控制协议

    RTCP*rtcp = RTCP::getInstance();
    NetworkMessageList *nm = NetworkMessageList::getInstance();
    QObject::connect(rtcp,&RTCP::sigNewMessage,
            nm,&NetworkMessageList::slotNewMessage);


    Widget w;
    w.show();
    a.exec();

    return 0;
}
