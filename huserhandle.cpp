#include "huserhandle.h"
#include "huserdbapi.h"
#include <QProcessEnvironment>
#include <QSettings>
#include <QString>
#include <QWidget>
#include "hpointseldlg.h"
#include "huserminitor.h"
#include "hprotocol.h"
#define DATA_TYPE_STATION  0x01
#define DATA_TYPE_DIGITAL  0x02
#define DATA_TYPE_ANALOGUE 0x03

HUserHandle::HUserHandle(QObject *parent) : QObject(parent)
{
    m_lpPluginCallback = NULL;
    m_strConfigFilePath = QProcessEnvironment::systemEnvironment().value(SYSENVIRONMENTVAR) + "/data/cdt.ini";
    m_settings = new QSettings(m_strConfigFilePath,QSettings::IniFormat);
    m_widget = NULL;
}

HUserHandle::~HUserHandle()
{
    //最好把STATION,ANALOGUE,DIGITAL结构换成简单点 ---huangw
    while(!m_stationList.isEmpty())
        delete (STATION*)m_stationList.takeFirst();
    while(!m_analogueList.isEmpty())
        delete (ANALOGUE*)m_analogueList.takeFirst();
    while(!m_digitalList.isEmpty())
        delete (DIGITAL*)m_digitalList.takeFirst();
}

void HUserHandle::initConfig()
{
    STATION station;
    PLUGINPARAM  Param;
    Param.wStation = (ushort)-1;
    Param.btType = TYPE_NULL;
    Param.pBuffer = &station;

    for(Param.wPoint = 0;m_lpPluginCallback(PM_FINDDBINFO,0,(HLPARAM)&Param);Param.wPoint++)//读取厂站的信息，从0开始，注意wPoint此时是厂站的ID
    {
        STATION *nstation = new STATION;
        if(!nstation) return;
        memcpy(nstation,&station,sizeof(STATION));
        m_stationList.append(nstation);
        if(nstation)
        {
            ANALOGUE analogue;
            PLUGINPARAM Param;
            Param.wStation = nstation->wStationID;
            Param.btType = TYPE_ANALOGUE;
            Param.pBuffer = &analogue;
            for(Param.wPoint = 0;m_lpPluginCallback(PM_FINDDBINFO,0,(HLPARAM)&Param);Param.wPoint++)
            {
                ANALOGUE* analogue1 = new ANALOGUE;
                memcpy(analogue1,&analogue,sizeof(ANALOGUE));
                m_analogueList.append(analogue1);
            }

            DIGITAL digital;
            Param.wStation = nstation->wStationID;
            Param.btType = TYPE_DIGITAL;
            Param.pBuffer = &digital;
            for(Param.wPoint = 0;m_lpPluginCallback(PM_FINDDBINFO,0,(HLPARAM)&Param);Param.wPoint++)
            {
                DIGITAL* digital1 = new DIGITAL;
                memcpy(digital1,&digital,sizeof(DIGITAL));
                m_digitalList.append(digital1);
            }
        }
    }
}

void HUserHandle::pluginConfig(LPPLUGINPROC lpPlugincallback)
{
    m_lpPluginCallback = lpPlugincallback;
    if(NULL == m_lpPluginCallback)
        return;
}

void HUserHandle::showConfig()
{
    initConfig();
    HPointSelDlg dlg(this);
    dlg.exec();
}

void HUserHandle::loadData()
{
    piConfig.m_wSendYxTime = m_settings->value("TIME/yxSendTime",30).toUInt();
    piConfig.m_wSendYcTime = m_settings->value("TIME/ycSendTime",30).toUInt();
    piConfig.m_strPortName = m_settings->value("SERIAL/portName","COM1").toString();
    piConfig.m_nPortBaudRate = m_settings->value("SERIAL/baudRate",9600).toInt();

    int nCount = m_settings->value("YXLIST",0).toUInt();
    for(int i = 0;i < nCount;i++)
    {
        QString key = QString("YXLIST/%1").arg(i);
        QString value = m_settings->value(key,"0,-1").toString();
        QStringList valList = value.split(',');
        if(valList.count() != 2) continue;
        POINTITEM * ptItem = new POINTITEM;
        ptItem->wStationNo = QString(valList[0]).toUShort();
        ptItem->wPointNo = QString(valList[1]).toUShort();
        piConfig.m_yxList.append(ptItem);
    }

    nCount = m_settings->value("YCLIST",0).toUInt();
    for(int i = 0;i < nCount;i++)
    {
        QString key = QString("YCLIST/%1").arg(i);
        QString value = m_settings->value(key,"0,-1").toString();
        QStringList valList = value.split(',');
        if(valList.count() != 2) continue;
        POINTITEM * ptItem = new POINTITEM;
        ptItem->wStationNo = QString(valList[0]).toUShort();
        ptItem->wPointNo = QString(valList[1]).toUShort();
        piConfig.m_ycList.append(ptItem);
    }

}

void HUserHandle::saveData()
{
    m_settings->setValue("TIME/yxSendTime",piConfig.m_wSendYxTime);
    m_settings->setValue("TIME/ycSendTime",piConfig.m_wSendYcTime);
    m_settings->setValue("SERIAL/portName",piConfig.m_strPortName);
    m_settings->setValue("SERIAL/baudRate",piConfig.m_nPortBaudRate);

    m_settings->setValue("YXLIST",piConfig.m_yxList.count());
    for(int i = 0;i < piConfig.m_yxList.count();i++)
    {
        POINTITEM * ptItem = piConfig.m_yxList[i];
        if(!ptItem) continue;
        QString key = QString("YXLIST/%1").arg(i);
        QString value = QString("%1,%2").arg(ptItem->wStationNo).arg(ptItem->wPointNo);
        m_settings->setValue(key,value);
    }

    m_settings->setValue("YCLIST",piConfig.m_ycList.count());
    for(int i = 0;i < piConfig.m_ycList.count();i++)
    {
        POINTITEM * ptItem = piConfig.m_ycList[i];
        if(!ptItem) continue;
        QString key = QString("YCLIST/%1").arg(i);
        QString value = QString("%1,%2").arg(ptItem->wStationNo).arg(ptItem->wPointNo);
        m_settings->setValue(key,value);
    }
}

void HUserHandle::initProc()
{
    HUserMinitor *dlg = new HUserMinitor;//启动报文监视窗口
    dlg->hide();
    //启动时间定时发送
}

void HUserHandle::exitProc()
{
    //清空列表
}

void HUserHandle::showMonitor()
{
    /*HUserMinitor* dlg;
    if(dlg->isHidden())
        dlg->show();*/
}

QString HUserHandle::getPointName(quint8 type,quint16 stNo,quint16 ptNo)
{
    QString strName = "";
    if(type == 0)
    {
     for(int i = 0;i < m_digitalList.count();i++)
     {
         DIGITAL* digital = m_digitalList[i];
         if(digital && digital->wStationID == stNo && digital->wDigitalID == ptNo)
         {
             strName = digital->szDigitalName;
             break;
         }
     }
    }
    else if(type == 1)
    {
     for(int i = 0;i < m_analogueList.count();i++)
     {
         ANALOGUE* analogue = m_analogueList[i];
         if(analogue && analogue->wStationID == stNo && analogue->wAnalogueID == ptNo)
         {
             strName = analogue->szAnalogueName;
             break;
         }
     }
    }
    return strName;
}

bool HUserHandle::findPointInDb(quint8 type,quint16 stNo,quint16 ptNo)
{
    bool bfind = false;
    if(type == 0)
    {
     for(int i = 0;i < piConfig.m_yxList.count();i++)
     {
         POINTITEM* pi = (POINTITEM*)piConfig.m_yxList[i];
         if(pi && pi->wStationNo == stNo && pi->wPointNo == ptNo)
         {
             bfind = true;
             break;
         }
     }
    }
    else if(type == 1)
    {
     for(int i = 0;i < piConfig.m_ycList.count();i++)
     {
         POINTITEM* pi = (POINTITEM*)piConfig.m_ycList[i];
         if(pi && pi->wStationNo == stNo && pi->wPointNo == ptNo)
         {
             bfind = true;
             break;
         }
     }
    }
    return bfind;
}

/////////////////////////////////////////////////////////////////////////////////
HUserHandle userHandle;
bool PLUGIN_EXPORT pluginProc(int type,unsigned int param1,long long param2)
{
    switch(type)
    {
    case PLG_PARENT:
    {
        QWidget* widget = NULL;
        userHandle.m_widget = widget;
        break;
    }
    case PLG_PROCNAME:
    {
        param1 = PLUGIN_ID_CDT;
        strcpy((char*)param2,"cdt");
        break;
    }
    case PLG_LOADDATA:
    {
        userHandle.loadData();
        break;
    }
    case PLG_SAVEDATA:
    {
        userHandle.saveData();
        break;
    }
    case PLG_INITPROC://初始化串口等信息
    {
        userHandle.initProc();
        break;
    }
    case PLG_EXITPROC:
        break;
    case PLG_CHANGEDYX:
        break;
    case PLG_PLUGINCONFIG://运行时也要设置回调函数，可以获取实时状态
    {
        userHandle.pluginConfig((LPPLUGINPROC)param2);
        break;
    }
    case PLG_SHOWMSGWIN://查看报文状态
    {
        userHandle.showMonitor();
        break;
    }
    case PLG_SHOWCONFIGWIN:
    {
        userHandle.showConfig();
        break;
    }
    default:
        break;
    }
    return true;
}
