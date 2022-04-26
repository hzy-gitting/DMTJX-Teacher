#ifndef TEACHERSNDMSGWINDOW_H
#define TEACHERSNDMSGWINDOW_H

#include <QMainWindow>

namespace Ui {
class TeacherSndMsgWindow;
}

class TeacherSndMsgWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TeacherSndMsgWindow(QWidget *parent = nullptr);
    ~TeacherSndMsgWindow();

private slots:
    void on_sendMsgBtn_clicked();

private:
    Ui::TeacherSndMsgWindow *ui;
};

#endif // TEACHERSNDMSGWINDOW_H
