#ifndef PARAMETERSETTTINGDIALOG_H
#define PARAMETERSETTTINGDIALOG_H

#include <QDialog>
#include<QHostAddress>

namespace Ui {
class ParameterSetttingDialog;
}

class ParameterSetttingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParameterSetttingDialog(QWidget *parent = nullptr);
    ~ParameterSetttingDialog();

private slots:
    void on_networkAdapterList_itemSelectionChanged();

    void on_saveSettingBtn_clicked();

    void on_closeBtn_clicked();

    void on_browseFileBtn_clicked();

private:
    Ui::ParameterSetttingDialog *ui;

    QHostAddress ipAddr;
    QString macAddr;
    QString fileRcvDir;

    quint16 videoPort;
    quint16 RTCPPort;

    void showNetworkAdapterList();
};

#endif // PARAMETERSETTTINGDIALOG_H
