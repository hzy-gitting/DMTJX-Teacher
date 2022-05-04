#include "netmessage.h"

const QHostAddress &NetMessage::getSenderAddr() const
{
    return senderAddr;
}

void NetMessage::setSenderAddr(const QHostAddress &newSenderAddr)
{
    senderAddr = newSenderAddr;
}

const QString &NetMessage::getContent() const
{
    return content;
}

void NetMessage::setContent(const QString &newContent)
{
    content = newContent;
}

const QDateTime &NetMessage::getDateTime() const
{
    return dateTime;
}

void NetMessage::setDateTime(const QDateTime &newDateTime)
{
    dateTime = newDateTime;
}

NetMessage::NetMessage()
{

}

NetMessage::NetMessage(QString cont, QDateTime dt, QHostAddress senderAddress):
    content(cont),
    dateTime(dt),
    senderAddr(senderAddress)
{

}
