#include "huserminitor.h"
#include "ui_userminitor.h"
#include "huserhandle.h"
#include "huserworker.h"
#include "hprotocol.h"
#include <QScrollBar>
HUserMinitor::HUserMinitor(HUserHandle* handle,QWidget *parent) :
    m_pUserHandle(handle),QMainWindow(parent),
    ui(new Ui::HUserMinitor)
{
    ui->setupUi(this); 
    openSerialPort();
    m_protocol = new HProtocol;
    connect(m_protocol,SIGNAL(sendData(const QByteArray&)),this,SLOT(handleWritten(const QByteArray&)));
    connect(&m_timer, &QTimer::timeout, this, &HUserMinitor::handleTimeout);
    m_timer.start(1000);
}

HUserMinitor::~HUserMinitor()
{
    delete ui;
}

void HUserMinitor::openSerialPort()
{
    m_serialPort = new QSerialPort(this);
    //m_serialPort->setPortName(m_pUserHandle->piConfig.m_strPortName);
    //m_serialPort->setBaudRate(m_pUserHandle->piConfig.m_nPortBaudRate);
    m_serialPort->setPortName("COM1");
    m_serialPort->setBaudRate(QSerialPort::Baud9600);
    m_serialPort->setDataBits(QSerialPort::Data8);//数据位宽 8
    m_serialPort->setParity(QSerialPort::NoParity);//无奇偶校验
    m_serialPort->setStopBits(QSerialPort::OneStop);//停止位宽1
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);//无控制流
    if(m_serialPort->open(QIODevice::ReadWrite))
    {
        //connect(m_serialPort, &QSerialPort::bytesWritten, this, &HUserMinitor::handleBytesWritten);
        connect(m_serialPort, &QSerialPort::readyRead, this, &HUserMinitor::handleReadyRead,Qt::DirectConnection);//阻塞式
        connect(m_serialPort, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
                this, &HUserMinitor::handleError);
    }

}

void HUserMinitor::closeSerialPort()
{
    if (m_serialPort->isOpen())
        m_serialPort->close();
}

void HUserMinitor::getShowData(const QByteArray &data)
{
    showData(data);
}

void HUserMinitor::handleWritten(const QByteArray& data)
{
    m_writeData = data;

    qint64 bytesWritten = m_serialPort->write(data);
    showData(data);

    if (bytesWritten == -1) {
        m_standardOutput << QObject::tr("Failed to write the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
        //QCoreApplication::exit(1);
    } else if (bytesWritten != m_writeData.size()) {
        m_standardOutput << QObject::tr("Failed to write all the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
        //QCoreApplication::exit(1);
    }
}

void HUserMinitor::handleReadyRead()
{
    //加到队列里面
    m_readData.append(m_serialPort->readAll());
    m_protocol->processFrame(m_readData);
    showData(m_readData);
}

void HUserMinitor::handleTimeout()
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

void HUserMinitor::handleError(QSerialPort::SerialPortError serialPortError)
{
    if (serialPortError == QSerialPort::WriteError) {
        m_standardOutput << QObject::tr("An I/O error occurred while writing the data to port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
        //QCoreApplication::exit(1);
    }

    if (serialPortError == QSerialPort::ReadError) {
        m_standardOutput << QObject::tr("An I/O error occurred while reading the data from port %1, error: %2").arg(m_serialPort->portName()).arg(m_serialPort->errorString()) << endl;
        //QCoreApplication::exit(1);
    }
}

void HUserMinitor::showData(const QByteArray &writeData)
{
    QString temp,temp1;
    for(int i=0;i<writeData.length();i++)
    {
         temp1 = QString("%1 ").arg((int)writeData[i],0,16);
         temp += temp1;
    }
    ui->textBrowser->insertPlainText(temp);
    QScrollBar *bar = ui->textBrowser->verticalScrollBar();
    bar->setValue(bar->maximum());
}
