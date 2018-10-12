#include "hserialport.h"
#include <QTime>
#include <QDebug>
#include <QTextCodec>
QT_USE_NAMESPACE
/***********************调用外部接口*****************************************/
extern void add_msg_for_show(int type,QByteArray& baData);
extern void add_data_for_recv(const QByteArray& baData);

/************************发送共享队列****************************************/
//处理规约层过来的消息
QMutex g_send_mutex;
std::list<SndData*> g_send_list;
void add_data_for_send(char* data,int len);
void  push_send_data_to_list(SndData *data);
SndData* pop_send_data_from_list();
void clear_send_data_from_list();
/************************************************************************/

HSerialPort::HSerialPort(QObject *parent)
    : QThread(parent), waitTimeout(5000), running(false)
{
    connect(this,&QThread::finished,this,&QThread::deleteLater);
}

HSerialPort::~HSerialPort()
{
    mutex.lock();
    running = true;
    mutex.unlock();
    clear_send_data_from_list();
    wait();
}

bool HSerialPort::initSerialPort(const QString& portName,const quint32 baudRate)
{
    this->portName = portName;
    this->baudRate = baudRate;
    m_serialPort.setPortName(portName);
    m_serialPort.setBaudRate(baudRate);
    m_serialPort.setDataBits(QSerialPort::Data8);//数据位宽 8
    m_serialPort.setParity(QSerialPort::NoParity);//无奇偶校验
    m_serialPort.setStopBits(QSerialPort::OneStop);//停止位宽1
    m_serialPort.setFlowControl(QSerialPort::NoFlowControl);//无控制流
    if (!m_serialPort.open(QIODevice::ReadWrite))
    {
        add_msg_for_show(M_ERROR,QByteArray("error:cannot open com!"));
        return false;
    }
    return true;
}

void HSerialPort::run()
{
    while (!running) {
        //qDebug()<<"SerialPortThread:" <<this->currentThreadId();
        SndData* send_data = pop_send_data_from_list();    
        if(send_data)
        {
            QByteArray requestData(send_data->data,send_data->len);
            m_serialPort.write(requestData);
            if (m_serialPort.waitForBytesWritten(waitTimeout))
            {
                if(send_data)
                {
                    if(send_data->data)
                        delete send_data->data;
                    delete send_data;
                    send_data = NULL;
                }
                add_msg_for_show(M_SEND,requestData);
            }
            else
            {
                add_msg_for_show(M_ERROR,QByteArray("error:send data fail!"));
            }
        }

        //mutex.lock();
        if (m_serialPort.waitForReadyRead(waitTimeout))
        {
            QByteArray responseData = m_serialPort.readAll();
            while (m_serialPort.waitForReadyRead(10))
                responseData += m_serialPort.readAll();
            add_data_for_recv(responseData);
            add_msg_for_show(M_RECV,responseData);
        }
        else
        {
            //add_msg_for_show(M_ERROR,"读取数据失败");
        }
        //mutex.unlock();
    }
}

void  push_send_data_to_list(SndData *data)
{
    g_send_mutex.lock();
    g_send_list.push_back(data);
    g_send_mutex.unlock();
}

SndData* pop_send_data_from_list()
{
    SndData* snd_data = NULL;
    g_send_mutex.lock();
    if(!g_send_list.empty())
    {
        snd_data = g_send_list.front();
        g_send_list.pop_front();
    }
    g_send_mutex.unlock();
    return snd_data;
}

void clear_send_data_from_list()
{
    g_send_mutex.lock();
    while(!g_send_list.empty())
    {
        SndData *snddata = g_send_list.front();
        g_send_list.pop_front();
        if(snddata)
        {
            if(snddata->data)
                delete[] snddata->data;
            delete snddata;
            snddata = NULL;
        }
    }
    g_send_mutex.unlock();
}

void add_data_for_send(char* data, int len)
{
    if(NULL == data || 0 == len)
        return;
    SndData* sndData = new SndData;
    memcpy(sndData->data,data,len);
    sndData->len = len;
    push_send_data_to_list(sndData);
}
