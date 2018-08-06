#ifndef HUSERWORKER_H
#define HUSERWORKER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

#include <QTextStream>
#include <QTimer>
#include <QByteArray>
#include <QThread>
class HProtocol;
class HSerialProt;
class HUserHandle;
class HUserWorker;
class HUserWorker : public QThread
{
    Q_OBJECT
public:
    explicit HUserWorker(QObject *parent = 0);

public:
    void openSerialPort();
    void closeSerialPort();
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
};

#endif // HUSERWORKER_H
