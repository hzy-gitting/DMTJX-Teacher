#include "parametersetttingdialog.h"
#include "ui_parametersetttingdialog.h"
#include<QNetworkInterface>
#include"systemconfigurationinfo.h"
#include<QFileDialog>

ParameterSetttingDialog::ParameterSetttingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParameterSetttingDialog)
{
    ui->setupUi(this);

    //加载系统配置参数
    QSettings *setting = SystemInfo::getSettings();
    fileRcvDir = setting->value("fileRcvPath").toString();
    macAddr = setting->value("macAddr").toString();
    ui->fileRcvPathEdit->setText(fileRcvDir);

    showNetworkAdapterList();
}

ParameterSetttingDialog::~ParameterSetttingDialog()
{
    delete ui;
}

//展示网络适配器列表
void ParameterSetttingDialog::showNetworkAdapterList()
{

    QSettings *setting = SystemInfo::getSettings();
    QString currentMacAddr = setting->value("macAddr").toString();
    QString sItem;
    //查找所有网络接口信息
    QList ifs = QNetworkInterface::allInterfaces();
    QNetworkAddressEntry addrEntry;
    for(int i=0;i<ifs.size();i++){
        sItem.append(QString::number(ifs[i].index()));  //索引号
        sItem.append(" ");
        sItem.append(ifs[i].humanReadableName());       //可读名称
        sItem.append(" ");
        sItem.append(ifs[i].hardwareAddress());     //硬件地址（MAC地址）
        sItem.append(" ");
        for(int j = 0;j<ifs[i].addressEntries().size();j++){
            addrEntry = ifs[i].addressEntries().at(j);
            //只显示IPv4地址
            if(addrEntry.ip().protocol() == QAbstractSocket::IPv4Protocol){
                sItem.append("(");
                sItem.append(addrEntry.ip().toString());    //IP
                sItem.append("/");
                sItem.append(addrEntry.netmask().toString());   //子网掩码
                sItem.append(")");
                QListWidgetItem *newItem = new QListWidgetItem;
                newItem->setData(Qt::UserRole,ifs[i].hardwareAddress());
                newItem->setText(sItem);
                newItem->setToolTip(sItem);
                ui->networkAdapterList->addItem(newItem);
                //选中系统当前使用的网卡
                if(ifs[i].hardwareAddress() == currentMacAddr){
                    ui->networkAdapterList->setCurrentItem(newItem);
                }
            }
        }
        sItem.clear();
    }
    ui->networkAdapterList->setVisible(true);
}

//更改网络适配器选择
void ParameterSetttingDialog::on_networkAdapterList_itemSelectionChanged()
{
    this->macAddr = ui->networkAdapterList->currentItem()->data(Qt::UserRole).toString();
    qDebug()<<macAddr;
}

//保存最新参数设置
void ParameterSetttingDialog::on_saveSettingBtn_clicked()
{
    QSettings *setting = SystemInfo::getSettings();
    //连接教室局域网的网卡硬件地址
    setting->setValue("macAddr",this->macAddr);
    setting->setValue("fileRcvPath",this->fileRcvDir);  //文件接收位置
    close();    //关闭窗口
}

//关闭窗口
void ParameterSetttingDialog::on_closeBtn_clicked()
{
    close();
}

//浏览文件按钮
void ParameterSetttingDialog::on_browseFileBtn_clicked()
{
    QString dir;
    dir = QFileDialog::getExistingDirectory(this,"选择文件接收文件夹位置");
    //没有选择，直接退出，保留原有设置
    if(dir.isEmpty()){
        return;
    }
    fileRcvDir = dir;
    ui->fileRcvPathEdit->setText(fileRcvDir);   //更改目录
}

