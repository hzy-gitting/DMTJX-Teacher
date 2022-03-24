#include "messageform.h"
#include "ui_messageform.h"

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
