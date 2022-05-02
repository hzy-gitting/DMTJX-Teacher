#ifndef SENDFILEDIALOG_H
#define SENDFILEDIALOG_H

#include <QDialog>
#include<QTcpServer>
namespace Ui {
class SendFileDialog;
}

class SendFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SendFileDialog(QWidget *parent = nullptr);
    ~SendFileDialog();

private slots:
    void on_chooseFileBtn_clicked();

    void on_sendBtn_clicked();

    void newConn();
    void on_closeBtn_clicked();

private:
    Ui::SendFileDialog *ui;

    QTcpServer *fsrv;
    QList<QTcpSocket*> socketList;
    QList<QString> fileList;//待发送文件列表
    QList<qint64> fileSizeList; //文件大小列表

    int nStudent;   //待发送的学生个数

    void startSendFileData();
};

#endif // SENDFILEDIALOG_H
