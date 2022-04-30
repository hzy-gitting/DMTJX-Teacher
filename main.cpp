#include "widget.h"

#include <QApplication>
#include"rtcp.h"
#include<QSettings>
#include"systemconfigurationinfo.h"

int main(int argc, char *argv[])
{
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

    Widget w;
    w.show();
    return a.exec();
}
