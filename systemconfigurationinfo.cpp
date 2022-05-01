#include "systemconfigurationinfo.h"
#include<QApplication>
#include<QMap>
#include<QNetworkInterface>

SystemInfo *SystemInfo::pInst = NULL;
QSettings *SystemInfo::settings = NULL;

const QHostAddress SystemInfo::getIpAddr() const
{
    return ipAddr;
}

SystemInfo::SystemInfo(QObject *parent) : QObject(parent)
{

}
//返回系统配置单例
QSettings *SystemInfo::getSettings()
{
    if(!settings){
        settings = new QSettings(
                    QApplication::applicationDirPath()+"/Settings.ini",
                    QSettings::IniFormat);
    }
    return settings;
}

SystemInfo *SystemInfo::getSystemInfo()
{
    if(!pInst){
        pInst = new SystemInfo;
    }
    return pInst;
}

bool SystemInfo::initialize()
{
    if(!settings){
        settings = new QSettings(
                    QApplication::applicationDirPath()+"/Settings.ini",
                    QSettings::IniFormat);
    }
    if(!pInst){
        pInst = new SystemInfo;
    }
    QMap<QString,QString> *defaultSettings = new QMap<QString,QString>();
    defaultSettings->insert("macAddr","");
    defaultSettings->insert("RTCP_port","8999");
    defaultSettings->insert("fileRcvPath",
                            QApplication::applicationDirPath()+"/teacherRcv");
    defaultSettings->insert("Vedio_port","8887");

    //设置默认配置
    for(QMap<QString,QString>::key_iterator it = defaultSettings->keyBegin();
        it!=defaultSettings->keyEnd();it++){
        if(!settings->contains(*it)){
            settings->setValue(*it,defaultSettings->value(*it));
        }
    }
    QString macAddr = settings->value("macAddr").toString();
    if(macAddr.isEmpty()){
        return true;
    }
    //查找所有网络接口信息
    QList ifs = QNetworkInterface::allInterfaces();
    QNetworkAddressEntry addrEntry;
    for(int i=0;i<ifs.size();i++){
        if(ifs[i].hardwareAddress() == macAddr){
            for(int j = 0;j<ifs[i].addressEntries().size();j++){
                addrEntry = ifs[i].addressEntries().at(j);
                //只显示IPv4地址
                if(addrEntry.ip().protocol() == QAbstractSocket::IPv4Protocol){
                    pInst->ipAddr = addrEntry.ip();
                }
            }
        }
    }
    //
    return true;
}
