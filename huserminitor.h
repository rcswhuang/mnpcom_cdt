#ifndef HUSERMINITOR_H
#define HUSERMINITOR_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QTextStream>
#include <QTimer>
#include <QByteArray>
#include <QThread>
class HProtocol;
class HSerialProt;
class HUserHandle;
class HUserWorker;
namespace Ui {
class HUserMinitor;
}

class HUserMinitor : public QMainWindow
{
    Q_OBJECT

public:
    explicit HUserMinitor(HUserHandle* handle,QWidget *parent = 0);
    ~HUserMinitor();
public slots:
    void getShowData(const QByteArray& data);
public:
    void openSerialPort();
    void closeSerialPort();
    void showData(const QByteArray& data);
private slots:
    void handleWritten(const QByteArray &writeData);
    void handleReadyRead();
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError error);
protected:
    void run();

signals:
    void addShowData(const QByteArray& showData);
private:
    HUserHandle* m_pUserHandle;
    QSerialPort *m_serialPort;
    HProtocol *m_protocol;
    QTimer          m_timer;

    QByteArray   m_writeData;
    QTextStream  m_standardOutput;
    qint64       m_bytesWritten;
    QByteArray  m_readData;
private:
    int m_nYxSendTime;
    int m_nYcSendTime;
private:
    Ui::HUserMinitor *ui;
};

#endif // HUSERMINITOR_H
