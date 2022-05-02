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

private slots:
    void on_tcpSendBtn_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void uservRDRD();
    void on_openFileRcvDirBtn_clicked();

    void on_remoteWakeBtn_clicked();

    void studentTableAddItem(int sId, QHostAddress ip, qint32 port, char macAddr[]);
    void setUpParam();
    void slotStuDisconnected(int sId);
    void on_studentListWidget_itemSelectionChanged();

private:
    Ui::Widget *ui;
    QUdpSocket *userv;

    QList<int> stuIDSelectedList;   //当前选中的学生id集合
public:
    void paintEvent(QPaintEvent *event) override;
};
#endif // WIDGET_H
