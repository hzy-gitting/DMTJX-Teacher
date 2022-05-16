#ifndef SENDSCREENVIDEODATATHREAD_H
#define SENDSCREENVIDEODATATHREAD_H

#include <QObject>

class SendScreenVideoDataThread : public QObject
{
    Q_OBJECT
public:
    explicit SendScreenVideoDataThread(QObject *parent = nullptr);

private:
    void onStop();
public slots:
    void start();
signals:

};

#endif // SENDSCREENVIDEODATATHREAD_H
