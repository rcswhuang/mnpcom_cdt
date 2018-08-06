#include "huserworker.h"
#include "hprotocol.h"
#include "huserhandle.h"
//处理线程类
HUserWorker::HUserWorker(QObject *parent) : QObject(parent)
{
    openSerialPort();
    m_protocol = new HProtocol;
    connect(m_protocol,SIGNAL(sendData(const QByteArray&)),this,SLOT(handleWritten(const QByteArray&)));
    connect(&m_timer, &QTimer::timeout, this, &HUserWorker::handleTimeout);
    m_timer.start(1000);
}


void HUserWorker::openSerialPort()
{
    m_serialPort = new QSerialPort(this);
    m_serialPort->setPortName(m_pUserHandle->piConfig.m_strPortName);
    m_serialPort->setBaudRate(m_pUserHandle->piConfig.m_nPortBaudRate);
    m_serialPort->setDataBits(QSerialPort::Data8);//数据位宽 8
    m_serialPort->setParity(QSerialPort::NoParity);//无奇偶校验
    m_serialPort->setStopBits(QSerialPort::OneStop);//停止位宽1
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//无控制流
    if(m_serialPort->open(QIODevice::ReadWrite))
    {
        //connect(m_serialPort, &QSerialPort::bytesWritten, this, &HUserMinitor::handleBytesWritten);
        connect(m_serialPort, &QSerialPort::readyRead, this, &HUserWorker::handleReadyRead);
        connect(m_serialPort, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
                this, &HUserWorker::handleError);
    }

}

void HUserWorker::closeSerialPort()
{
    if (m_serialPort->isOpen())
        m_serialPort->close();
}


void HUserWorker::handleWritten(const QByteArray& data)
{
    m_writeData = writeData;

    qint64 bytesWritten = m_serialPort->write(writeData);

    if (bytesWritten == -1) {
        m_standardOutput << QObject::tr("Failed to write the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
        //QCoreApplication::exit(1);
    } else if (bytesWritten != m_writeData.size()) {
        m_standardOutput << QObject::tr("Failed to write all the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
        //QCoreApplication::exit(1);
    }
}

void HUserWorker::handleReadyRead()
{
    //加到队列里面
    m_readData.append(m_serialPort->readAll());
    m_protocol->processFrame(m_readData);
}

void HUserWorker::handleTimeout()
{
    static ushort wSendYxTime = 0;
    static ushort wSendYcTime = 0;
    if(wSendYxTime < m_nYxSendTime)
        wSendYxTime++;
    else
    {
        m_protocol->sendAllYx();
        wSendYxTime = 0;
    }

    if(wSendYcTime < m_nYcSendTime)
        wSendYcTime++;
    else
    {
        m_protocol->sendAllYc();
        wSendYcTime = 0;
    }
}

void HUserWorker::handleError(QSerialPort::SerialPortError error)
{

}
