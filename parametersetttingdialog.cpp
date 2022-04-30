#include "parametersetttingdialog.h"
#include "ui_parametersetttingdialog.h"
#include<QNetworkInterface>

ParameterSetttingDialog::ParameterSetttingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParameterSetttingDialog)
{
    ui->setupUi(this);

    showNetworkAdapterList();
}

ParameterSetttingDialog::~ParameterSetttingDialog()
{
    delete ui;
}

//展示网络适配器列表
void ParameterSetttingDialog::showNetworkAdapterList()
{

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
                newItem->setData(Qt::UserRole,ifs[i].hardwareAddress());
                newItem->setText(sItem);
                newItem->setToolTip(sItem);
                ui->networkAdapterList->addItem(newItem);
            }
        }
        sItem.clear();
    }
    ui->networkAdapterList->setVisible(true);
}

//更改网络适配器选择
void ParameterSetttingDialog::on_networkAdapterList_itemSelectionChanged()
{
    qDebug()<<__FUNCTION__;
    qDebug() << ui->networkAdapterList->currentItem()->data(Qt::UserRole);
}

//保存最新参数设置
void ParameterSetttingDialog::on_saveSettingBtn_clicked()
{

}

//关闭窗口
void ParameterSetttingDialog::on_closeBtn_clicked()
{
    close();
}

