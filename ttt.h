#ifndef TTT_H
#define TTT_H

#include <QObject>

class ttt : public QObject
{
    Q_OBJECT
public:
    explicit ttt(QObject *parent = nullptr);

signals:

};

#endif // TTT_H
