#include "networkutils.h"
#include<QNetworkDatagram>
#include<Windows.h>

#include<iphlpapi.h>

NetworkUtils::NetworkUtils()
{

}

bool NetworkUtils::getMacAddrByIp(char *macAddr,QHostAddress ip)
{
    PMIB_IPNETTABLE pINTab = NULL;
    ULONG tabSize = 0;
    ULONG ret = GetIpNetTable(pINTab,&tabSize,TRUE);    //试探缓冲区大小
    pINTab = (PMIB_IPNETTABLE )malloc(tabSize); //分配缓冲区
    if(!pINTab){
        return false;
    }
    ret = GetIpNetTable(pINTab,&tabSize,TRUE);  //获取系统所有ARP表项

    //错误处理
    switch(ret){
    case ERROR_NO_DATA:
        qDebug()<<"ERROR_NO_DATA";
        free(pINTab);
        return false;
        break;
    case ERROR_INSUFFICIENT_BUFFER:
        qDebug()<<"ERROR_INSUFFICIENT_BUFFER";
        free(pINTab);
        return false;
        break;
    case ERROR_INVALID_PARAMETER:
        qDebug()<<"ERROR_INVALID_PARAMETER";
        free(pINTab);
        return false;
        break;
    case ERROR_NOT_SUPPORTED:
        qDebug()<<"ERROR_NOT_SUPPORTED";
        free(pINTab);
        return false;
        break;
    case NO_ERROR:
        break;
    default:
        qDebug()<<"unkwon error";
        free(pINTab);
        return false; break;
    }

    bool bFound = false;

    //从IPNETTTABLE查找目的ip
    QHostAddress hostAddr;
    for(int i=0;i<pINTab->dwNumEntries;i++){
        struct in_addr addr;
        addr.S_un.S_addr = pINTab->table[i].dwAddr;
        hostAddr.setAddress(inet_ntoa(addr));
        if(hostAddr.isEqual(ip)){   //找到需要查询的ip了
            bFound = true;
            for(int j=0;j<pINTab->table[i].dwPhysAddrLen;j++){
                macAddr[j] = pINTab->table[i].bPhysAddr[j];     //拷贝mac地址
            }
            break;
        }
     }
    free(pINTab);
    return bFound;
}

bool NetworkUtils::sendMagicPacket(QUdpSocket *udpSocket, char macAddr[])
{
    QNetworkDatagram *magicPacket = new QNetworkDatagram;
    magicPacket->setDestination(QHostAddress::Broadcast,9);     //幻数据包以广播的方式发送出去
    //构造数据包体
    QByteArray data;
    data.append(6,(char)0xFF);  //（FF FF FF FF FF FF FF）开头
    for(int i=0;i<16;i++){
        data.append(macAddr,6);     //重复16次被唤醒主机MAC地址
    }
    magicPacket->setData(data);
    qint64 ret = udpSocket->writeDatagram(*magicPacket);
    delete magicPacket;
    if(ret == -1){
        qDebug()<<"发送唤醒数据包失败 "<< udpSocket->errorString();
        return false;
    }
    return true;
}

QString NetworkUtils::macToString(const char macAddr[])
{
    QString s;
    for(int i=0;i<6;i++){
        s.append(QString::number((unsigned int)(unsigned char)macAddr[i],16));  //十六进制输出
        if(i != 5){
            s.append(':');  //冒号分隔字节
        }
    }
    return s;
}
