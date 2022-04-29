#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H

#include<QHostAddress>
#include<QUdpSocket>

//网络工具类
class NetworkUtils
{
private:
    NetworkUtils();
public:

    static void getAllInterfaces();//获取网络接口信息

    static bool getMacAddrByIp(char *macAddr,QHostAddress ip);

    static bool sendMagicPacket(QUdpSocket *udpSocket, char macAddr[]);

    static QString macToString(const char macAddr[]);

};

#endif // NETWORKUTILS_H
