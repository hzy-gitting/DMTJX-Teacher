#ifndef NETMESSAGE_H
#define NETMESSAGE_H
#include<QDateTime>
#include<QHostAddress>

class NetMessage
{
private:
    QString content;        //消息内容
    QDateTime dateTime;     //发送的日期时间
    QHostAddress senderAddr;//发送者的主机地址
    bool isSelf;             //这条消息是否是自己发送的

public:
    NetMessage();
    NetMessage(QString cont,QDateTime dt,QHostAddress senderAddress = QHostAddress::LocalHost);
    const QHostAddress &getSenderAddr() const;
    void setSenderAddr(const QHostAddress &newSenderAddr);
    const QString &getContent() const;
    void setContent(const QString &newContent);
    const QDateTime &getDateTime() const;
    void setDateTime(const QDateTime &newDateTime);
};

#endif // NETMESSAGE_H
