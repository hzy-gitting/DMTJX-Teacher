#ifndef PARAMETERSETTTINGDIALOG_H
#define PARAMETERSETTTINGDIALOG_H

#include <QDialog>

namespace Ui {
class ParameterSetttingDialog;
}

class ParameterSetttingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParameterSetttingDialog(QWidget *parent = nullptr);
    ~ParameterSetttingDialog();

private:
    Ui::ParameterSetttingDialog *ui;
};

#endif // PARAMETERSETTTINGDIALOG_H
