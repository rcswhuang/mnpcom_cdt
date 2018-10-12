#include "huserminitor.h"
#include "ui_userminitor.h"
#include "huserhandle.h"
#include "hserialport.h"
#include "hprotocol.h"
#include <QScrollBar>
#include <QTextCodec>
#include <QDebug>
/***********************************************************/
QMutex g_show_mutex;
std::list<ShowData*> g_show_list;
void add_msg_to_show_list(ShowData* recv_data);
ShowData* remove_msg_from_show_list();
void clear_show_list();
void add_msg_for_show(int type,QByteArray& baData);
/************************************************************/
extern HUserHandle userHandle;

HUserMinitor::HUserMinitor(QWidget *parent) :QMainWindow(parent),
    ui(new Ui::HUserMinitor)
{
    ui->setupUi(this); 
    m_protocol = new HProtocol;
    m_protocol->start();
    m_SerialThread = new HSerialPort(this);
    if(m_SerialThread->initSerialPort(userHandle.piConfig.m_strPortName,userHandle.piConfig.m_nPortBaudRate))
        m_SerialThread->start();
    connect(&m_timer,&QTimer::timeout,this,&HUserMinitor::handleTimeout);
    m_timer.start(1000);
    //qDebug()<<"HUserMinitor:" <<QThread::currentThreadId();
}

HUserMinitor::~HUserMinitor()
{
    delete ui;
    delete m_protocol;
    delete m_SerialThread;
}


void HUserMinitor::handleTimeout()
{
    ShowData* sData = remove_msg_from_show_list();
    while(sData)
    {
        showData(sData);
        if(sData->data) delete[] sData->data;
        delete sData;
        sData = remove_msg_from_show_list();
    }
}

void HUserMinitor::showData(const ShowData* show_data)
{
    QString temp,temp1;
    char* tempChar = NULL;
    if(show_data->type == M_ERROR)
    {
        char* tempChar = new char[show_data->len+1];
        strcpy(tempChar,show_data->data);
        tempChar[show_data->len] = '\0';
        temp = QString(tempChar);
    }
    else
    {
        for(int i = 0;i < show_data->len;i++)
        {
            uchar value = (uchar)show_data->data[i];
            temp1 = QString("%1 ").arg(value,2,16,QLatin1Char('0'));
            temp += temp1;
        }
    }
    temp += " \n";
    ui->textBrowser->insertPlainText(temp);
    QScrollBar *bar = ui->textBrowser->verticalScrollBar();
    bar->setValue(bar->maximum());
    if(tempChar)
    {
        delete tempChar;
        tempChar = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void add_msg_to_show_list(ShowData* show_data)
{
    g_show_mutex.lock();
    while(g_show_list.size() > recvMax)
    {
        ShowData* rdata = g_show_list.front();
        g_show_list.pop_front();
        if(rdata->data)
            delete[] rdata->data;
        delete rdata;
    }
    g_show_list.push_back(show_data);
    g_show_mutex.unlock();
}

ShowData* remove_msg_from_show_list()
{
    ShowData* rdata = NULL;
    g_show_mutex.lock();
    if(!g_show_list.empty())
    {
        rdata = g_show_list.front();
        g_show_list.pop_front();
    }
    g_show_mutex.unlock();
    return rdata;
}

void clear_show_list()
{
    g_show_mutex.lock();
    while(!g_show_list.empty())
    {
        ShowData* rdata = g_show_list.front();
        g_show_list.pop_front();
        if(rdata->data)
            delete[] rdata->data;
        delete rdata;
    }
    g_show_list.clear();
    g_show_mutex.unlock();
}

void add_msg_for_show(int type,QByteArray& baData)
{
    int len = baData.length();
    ShowData* sData = new ShowData;
    sData->data = new char[len];
    sData->len = len;
    sData->type = type;
    memcpy(sData->data,baData.data(),len);
    add_msg_to_show_list(sData);
}
