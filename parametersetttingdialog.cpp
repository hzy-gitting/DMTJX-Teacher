#include "parametersetttingdialog.h"
#include "ui_parametersetttingdialog.h"
#include<QNetworkInterface>

ParameterSetttingDialog::ParameterSetttingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParameterSetttingDialog)
{
    ui->setupUi(this);

    QString sItem;
    //查找所有网络接口信息
    QList ifs = QNetworkInterface::allInterfaces();
    QNetworkAddressEntry addrEntry;
    for(int i=0;i<ifs.size();i++){
        sItem.append(QString::number(ifs[i].index()));
        sItem.append(" ");
        sItem.append(ifs[i].humanReadableName());
        sItem.append(" ");
        sItem.append(ifs[i].hardwareAddress());
        sItem.append(" ");
        for(int j = 0;j<ifs[i].addressEntries().size();j++){
            addrEntry = ifs[i].addressEntries().at(j);
            //只显示IPv4地址
            if(addrEntry.ip().protocol() == QAbstractSocket::IPv4Protocol){
                sItem.append("(");
                sItem.append(addrEntry.ip().toString());
                sItem.append("/");
                sItem.append(addrEntry.netmask().toString());
                sItem.append(")");
                QListWidgetItem *newItem = new QListWidgetItem;
                newItem->setText(sItem);
                newItem->setToolTip(sItem);
                ui->networkAdapterList->addItem(newItem);
            }
        }
        sItem.clear();
    }
}

ParameterSetttingDialog::~ParameterSetttingDialog()
{
    delete ui;
}
