#include "teachersndmsgwindow.h"
#include "ui_teachersndmsgwindow.h"
#include"widget.h"
#include"rtcp.h"

TeacherSndMsgWindow::TeacherSndMsgWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TeacherSndMsgWindow)
{
    ui->setupUi(this);
}

TeacherSndMsgWindow::~TeacherSndMsgWindow()
{
    delete ui;
}

void TeacherSndMsgWindow::on_sendMsgBtn_clicked()
{

    RTCP *rtcp = RTCP::getInstance();

    QString msg = ui->msgEdit->toPlainText();
    rtcp->sendMessage(msg.toUtf8());
}

