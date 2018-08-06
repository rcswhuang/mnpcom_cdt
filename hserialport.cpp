#include "hserialport.h"
#include "huserhandle.h"
#include <QCoreApplication>

QT_USE_NAMESPACE

HSerialPort::HSerialPort( QObject *parent)
    : QObject(parent)
    , m_standardOutput(stdout)
    , m_bytesWritten(0)
{

    m_timer.setSingleShot(true);

    connect(&m_timer, &QTimer::timeout, this, &HSerialPort::handleTimeout);
}

HSerialPort::~HSerialPort()
{
}

bool HSerialPort::initSerialPort()
{
    bool bok = false;
    m_serialPort = new QSerialPort;
    m_serialPort->setPortName(m_pUserHandle->piConfig.m_strPortName);
    m_serialPort->setBaudRate(m_pUserHandle->piConfig.m_nPortBaudRate);
    m_serialPort->setDataBits(QSerialPort::Data8);//数据位宽 8
    m_serialPort->setParity(QSerialPort::NoParity);//无奇偶校验
    m_serialPort->setStopBits(QSerialPort::OneStop);//停止位宽1
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//无控制流
    if(m_serialPort->open(QIODevice::ReadWrite))
    {
        bok = true;
    }
    else
    {

    }

    connect(m_serialPort, &QSerialPort::bytesWritten, this, &HSerialPort::handleBytesWritten);
    connect(m_serialPort, &QSerialPort::readyRead, this, &HSerialPort::handleReadyRead);
    connect(m_serialPort, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &HSerialPort::handleError);

    return bok;
}

void HSerialPort::closeSerialPort()
{
    if (m_serialPort->isOpen())
        m_serialPort->close();
}


void HSerialPort::handleBytesWritten(qint64 bytes)
{
    m_bytesWritten += bytes;
    if (m_bytesWritten == m_writeData.size()) {
        m_bytesWritten = 0;
        m_standardOutput << QObject::tr("Data successfully sent to port %1").arg(m_serialPort->portName()) << endl;
        QCoreApplication::quit();
    }
}

void HSerialPort::handleTimeout()
{
    m_standardOutput << QObject::tr("Operation timed out for port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
    QCoreApplication::exit(1);
}

void HSerialPort::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::WriteError) {
        m_standardOutput << QObject::tr("An I/O error occurred while writing the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
        QCoreApplication::exit(1);
    }
    
    if (serialPortError == QSerialPort::ReadError) {
        m_standardOutput << QObject::tr("An I/O error occurred while reading the data from port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
        QCoreApplication::exit(1);
    }
}

void HSerialPort::handleReadyRead()
{
    m_readData.append(m_serialPort->readAll());

    if (!m_timer.isActive())
        m_timer.start(5000);
}

void HSerialPort::write(const QByteArray &writeData)
{
    m_writeData = writeData;

    qint64 bytesWritten = m_serialPort->write(writeData);

    if (bytesWritten == -1) {
        m_standardOutput << QObject::tr("Failed to write the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
        QCoreApplication::exit(1);
    } else if (bytesWritten != m_writeData.size()) {
        m_standardOutput << QObject::tr("Failed to write all the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
        QCoreApplication::exit(1);
    }

    m_timer.start(5000);
}
