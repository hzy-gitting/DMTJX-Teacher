#include "networkmessagelist.h"
NetworkMessageList *NetworkMessageList::pInst = NULL;

NetworkMessageList::NetworkMessageList(QObject *parent) : QObject(parent)
{

}

NetworkMessageList *NetworkMessageList::getInstance()
{
    if(!pInst){
        pInst = new NetworkMessageList;
    }
    return pInst;
}

void NetworkMessageList::insertMessage(const NetMessage &msg)
{
    msgList.append(msg);
    emit newMessage();  //触发新消息信号
}

void NetworkMessageList::clearMessage()
{
    msgList.clear();

    emit sigClearMessage(); //触发清空消息信号
}

QList<NetMessage> NetworkMessageList::getAllMessage()
{
    return msgList;
}

void NetworkMessageList::slotNewMessage(const QByteArray &payload,const QHostAddress &senderAddr)
{
    qDebug()<<__FUNCTION__;
    NetMessage msg;
    msg.setContent(QString::fromUtf8(payload));
    msg.setDateTime(QDateTime::currentDateTime());
    msg.setSenderAddr(senderAddr);
    insertMessage(msg);
    qDebug()<<__FUNCTION__<<" end";
}
