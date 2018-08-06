#ifndef HSERIALPORT_H
#define HSERIALPORT_H

#include <QtSerialPort/QSerialPort>

#include <QTextStream>
#include <QTimer>
#include <QByteArray>
#include <QObject>

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE
class HUserHandle;
class HSerialPort : public QObject
{
    Q_OBJECT

public:
    HSerialPort(QObject *parent = 0);
    ~HSerialPort();

public:
    bool initSerialPort();
    void closeSerialPort();

    void write(const QByteArray &writeData);

private slots:
    void handleBytesWritten(qint64 bytes);
    void handleReadyRead();
    void handleTimeout();
    void handleError(QSerialPort::SerialPortError error);
    
    
private:
    HUserHandle* m_pUserHandle;
    QSerialPort *m_serialPort;
    QByteArray   m_writeData;
    QTextStream  m_standardOutput;
    qint64       m_bytesWritten;
    
    QByteArray  m_readData;
     
    QTimer          m_timer;
};

#endif
