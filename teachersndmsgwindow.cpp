#include "teachersndmsgwindow.h"
#include "ui_teachersndmsgwindow.h"
#include"widget.h"
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

    QString msg = ui->msgEdit->toPlainText();
    Widget *pa = (Widget*)this->parent();
    msg = "msg" + msg;
    pa->controlSocket->write(msg.toUtf8());
}

