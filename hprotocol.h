#ifndef HPROTOCOL_H
#define HPROTOCOL_H

#include <QThread>
#include <QTimer>

//工作线程
class Worker : public QObject
{
    Q_OBJECT
public:
    void sendAllYx();
    void sendAllYc();
    void sendChangeYx();
    void processFrame();
public slots:
    void handleTimeout();

};


class HProtocol : public QThread
{
    Q_OBJECT
public:
    explicit HProtocol(QObject *parent = 0);
    ~HProtocol();

public:
    void run();
};

#endif // HPROTOCOL_H
