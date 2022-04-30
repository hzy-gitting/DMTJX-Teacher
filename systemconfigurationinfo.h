#ifndef SYSTEMCONFIGURATIONINFO_H
#define SYSTEMCONFIGURATIONINFO_H
#include<QObject>
#include<QHostAddress>
#include<QSettings>

class SystemInfo : public QObject
{
    Q_OBJECT

private:
    QHostAddress ipAddr;  //本机IP
    QString macAddr;       //使用的网卡的硬件地址（mac地址）
    qint32 RTCPport;        //控制协议端口号
    QString fileRcvPath;    //文件接收目录
    static SystemInfo *pInst;
    static QSettings *settings;

    explicit SystemInfo(QObject *parent = nullptr);
public:

    static QSettings* getSettings();
    static SystemInfo* getSystemInfo();

    static bool initialize();
    bool load();

    bool save();

    QVariant get(const QString &key,const QVariant &defaultValue = QVariant());
    const QHostAddress getIpAddr() const;

signals:

};

#endif // SYSTEMCONFIGURATIONINFO_H
