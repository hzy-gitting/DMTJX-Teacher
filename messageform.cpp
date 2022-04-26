#include "messageform.h"
#include "ui_messageform.h"
#include"widget.h"
MessageForm::MessageForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessageForm)
{
    ui->setupUi(this);
}

MessageForm::~MessageForm()
{
    delete ui;
}

//发送消息按钮
void MessageForm::on_sendMsgBtn_clicked()
{
    QString msg = ui->msgEdit->toPlainText();
    Widget *pa = (Widget*)this->parent();
    msg = "msg" + msg;
    pa->controlSocket->write(msg.toUtf8());
}

