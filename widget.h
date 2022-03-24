#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QUdpSocket>
#include<QTcpSocket>
#include"network/networkcommunicationsystem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void stuDetect();
public slots:
    void tcpConnected();
    void tcpReadyRead();
    void tcpError();
    void written(qint64 bytesWritten);
private slots:
    void on_pushButton_clicked();

    void on_bindButton_clicked();

    void on_connectButton_clicked();

    void on_tcpSendBtn_clicked();

    void on_chooseFileBtn_clicked();

    void on_previewFileBtn_clicked();

    void on_tcpSendFileBtn_clicked();

    void dataSent(qint64 bytesSent);
    void on_btnSaveScreen_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::Widget *ui;
    QUdpSocket *us;
    QTcpSocket *ts;
    NetworkCommunicationSystem nsys;
    QScreen *screen;

    QTcpSocket *controlSocket;
    void paintEvent(QPaintEvent *event) override;
};
#endif // WIDGET_H
