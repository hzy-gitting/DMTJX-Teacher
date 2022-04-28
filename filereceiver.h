#ifndef FILERECEIVER_H
#define FILERECEIVER_H

#include <QObject>
#include<QFile>
#include<QTcpSocket>

struct fileRcv{
    QString name;
    qint64 size;
    qint64 sizeLeft;
    QFile *f;
};

//文件接收者：负责把网络接收到的数据保存到本地文件
class FileReceiver : public QObject
{
    Q_OBJECT

private:
    QList<fileRcv> frs;
    int n;  //正在接收第n个文件
    int state ;//状态

public:
    explicit FileReceiver(QObject *parent = nullptr);

    FileReceiver(QList<QString> fileList,QList<qint64> fileSizeList);

    void writeFile(QByteArray fdata);
signals:
    void fileRcvProgressUpdate(int n,qint64 sizeLeft);
    void fileAllRcvFinished();     //所有文件保存完毕
};

#endif // FILERECEIVER_H
