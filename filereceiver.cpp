#include "filereceiver.h"
#include"systemconfigurationinfo.h"

FileReceiver::FileReceiver(QObject *parent) : QObject(parent)
{

}
FileReceiver::FileReceiver(QList<QString> fileList,QList<qint64> fileSizeList){
    n = 0;
    state = 0;
    QSettings *setting = SystemInfo::getSettings();
    QString fileRcvDir = setting->value("fileRcvPath").toString();
    for(int i=0;i<fileList.size();i++){
        fileRcv fr;
        QString fileName = fileList.at(i);
        fr.name = fileName.sliced(fileName.lastIndexOf('/')+1);
        fr.size = fileSizeList.at(i);
        fr.sizeLeft = fr.size;

        fr.f = new QFile(fileRcvDir+"/"+fr.name);
        if(!fr.f->open(QIODevice::WriteOnly)){
            qDebug()<<"打开"<<fr.f->fileName()<<"失败";
        }
        frs.append(fr);
    }
    qDebug()<<"FileRcvWindow";
    //FileRcvWindow *frw = new FileRcvWindow(fileList,fileSizeList,this);

    //frw->show();
}

void FileReceiver::writeFile(QByteArray fdata){
    if(n >= frs.size()){
        fileAllRcvFinished();
        qDebug()<<"所有文件接收完毕";
        return;
    }
    fileRcv &fr = frs[n];

    fr.f->write(fdata);
    fr.sizeLeft -= fdata.length();
    fileRcvProgressUpdate(n,fr.sizeLeft);
    qDebug()<<"接收到"<<fdata.length()<<" 文件字节 当前文件剩余"<<fr.sizeLeft;
    if(fr.sizeLeft <= 0){
        qDebug()<< "文件"<<fr.name<<" 已接受完毕";
        fr.f->close();  //关闭该文件
        n++;        //准备接下一个文件
        if(n >= frs.size()){
            fileAllRcvFinished();
            qDebug()<<"所有文件接收完毕";
            return;
        }
    }
}
