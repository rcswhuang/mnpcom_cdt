#ifndef HUSERHANDLE_H
#define HUSERHANDLE_H
#include <QObject>
#include <QMetaType>
#include <QTimer>
#include "huserdbapi.h"
class QSettings;
typedef struct _tagpointItem
{
    quint16 wStationNo;
    quint16 wPointNo;
}POINTITEM;

Q_DECLARE_METATYPE(POINTITEM*)

typedef struct _tagPluginConfig
{
    quint16 m_wSendYxTime;
    quint16 m_wSendYcTime;
    QString m_strPortName;
    qint32  m_nPortBaudRate;

    QList<POINTITEM*> m_yxList;
    QList<POINTITEM*> m_ycList;
}PLUGINCONFIG;

class HUserHandle : public QObject
{
    Q_OBJECT
public:
    explicit HUserHandle(QObject *parent = 0);
    ~HUserHandle();

public:
    void initConfig();
    void pluginConfig(LPPLUGINPROC lpPluginProc);
    void showConfig();
    void loadData();
    void saveData();
    void initProc();
    void exitProc();
    void showMonitor();

public:
    QString getPointName(quint8 type,quint16 stNo,quint16 ptNo);
    bool findPointInDb(quint8 type,quint16 stNo,quint16 ptNo);
    void SendChangeYx();
signals:

public:
    QString m_strConfigFilePath;
    LPPLUGINPROC m_lpPluginCallback;
    QList<STATION*> m_stationList;
    QList<ANALOGUE*> m_analogueList;
    QList<DIGITAL*> m_digitalList;
public:
    PLUGINCONFIG piConfig;
    QSettings  *m_settings;
    QTimer m_Timer;
private:
    QString strUserName;
};

#endif // HUSERHANDLE_H
