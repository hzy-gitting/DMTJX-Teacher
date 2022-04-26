#include "widget.h"

#include <QApplication>
#include"rtcp.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setbuf(stdout, NULL);

    RTCP::start();  //启动远程教学控制协议

    Widget w;
    w.show();
    return a.exec();
}
