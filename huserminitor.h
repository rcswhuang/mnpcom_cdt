#ifndef HUSERMINITOR_H
#define HUSERMINITOR_H

#include <QMainWindow>
#include <QTimer>
#include "hmsgglobal.h"
class HProtocol;
class HSerialPort;

namespace Ui {
class HUserMinitor;
}
/*
 * 通讯模式：本应用中主界面线程，规约处理线程，串口接收线程之前的数据都是采用共享队列实现的。
 * 缺点就是:每个处理线程都有1个共享队列,要多写代码。
 * 其实还可以采用Qt的信号槽机制来实现。特别是串口接收线程和主界面显示比较适合用这种机制。也是比较可靠的。
*/

class HUserMinitor : public QMainWindow
{
    Q_OBJECT

public:
    explicit HUserMinitor(QWidget *parent = 0);
    ~HUserMinitor();

public:
    void showData(const ShowData* data);

public slots:
    void handleTimeout();
private:
    HProtocol *m_protocol;
    HSerialPort* m_SerialThread;
    QTimer m_timer;

private:
    Ui::HUserMinitor *ui;
};

#endif // HUSERMINITOR_H
