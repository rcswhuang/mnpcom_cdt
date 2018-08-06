#include "hpointinputdlg.h"
#include "ui_pointinputdlg.h"
#include "huserhandle.h"
HPointInputDlg::HPointInputDlg(quint8 type,HUserHandle* handle,QWidget *parent) :
    m_btPtType(type),m_pUserHandle(handle),QDialog(parent),
    ui(new Ui::pointInputDlg)
{
    ui->setupUi(this);
    init();
}

HPointInputDlg::~HPointInputDlg()
{
    delete ui;
}

void HPointInputDlg::init()
{
    //初始化
    if(!m_pUserHandle) return;
    ui->sendPtlistWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->ptListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(ui->stComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(stComboBox_changed()));
    connect(ui->addBtn,SIGNAL(clicked(bool)),this,SLOT(addBtn_clicked()));
    connect(ui->addAllBtn,SIGNAL(clicked(bool)),this,SLOT(addAllBtn_clicked()));
    connect(ui->delBtn,SIGNAL(clicked(bool)),this,SLOT(delBtn_clicked()));
    connect(ui->delAllBtn,SIGNAL(clicked(bool)),this,SLOT(delAllBtn_clicked()));
    connect(ui->okBtn,SIGNAL(clicked(bool)),this,SLOT(accept()));
    ui->stComboBox->clear();
    for(int i = 0;i < m_pUserHandle->m_stationList.count();i++)
    {
        STATION* station = m_pUserHandle->m_stationList[i];
        ui->stComboBox->addItem(station->szStationName,station->wStationID);
    }
    ui->stComboBox->setCurrentIndex(0);
    //initPoint();
}

void HPointInputDlg::initPoint()
{
    ui->sendPtlistWidget->clear();
    if(m_btPtType == 0)
    {
        for(int i = 0;i < m_pUserHandle->piConfig.m_yxList.count();i++)
        {
            POINTITEM* pi = m_pUserHandle->piConfig.m_yxList[i];
            QString strText = m_pUserHandle->getPointName(m_btPtType,pi->wStationNo,pi->wPointNo);
            QListWidgetItem* item = new QListWidgetItem;
            item->setText(strText);
            QVariant var;
            var.setValue(pi);
            item->setData(Qt::UserRole,var);
            ui->sendPtlistWidget->insertItem(i,item);
        }
    }
    else if(m_btPtType == 1)
    {
        for(int i = 0;i < m_pUserHandle->piConfig.m_ycList.count();i++)
        {
            POINTITEM* pi = m_pUserHandle->piConfig.m_ycList[i];
            QString strText = m_pUserHandle->getPointName(m_btPtType,pi->wStationNo,pi->wPointNo);
            QListWidgetItem* item = new QListWidgetItem;
            item->setText(strText);
            QVariant var;
            var.setValue(pi);
            item->setData(Qt::UserRole,var);
            ui->sendPtlistWidget->insertItem(i,item);
        }
    }
}

void HPointInputDlg::stComboBox_changed()
{
    if(ui->stComboBox->currentIndex() == (int)-1)
        return;
    initPoint();
    ui->ptListWidget->clear();
    quint16 wStationID = ui->stComboBox->currentData().toUInt();
    if(m_btPtType == 0)//yx
    {
        for(int i = 0;i < m_pUserHandle->m_digitalList.count();i++)
        {
            DIGITAL *digital = m_pUserHandle->m_digitalList[i];
            if(digital && digital->wStationID == wStationID && !m_pUserHandle->findPointInDb(m_btPtType,digital->wStationID,digital->wDigitalID))
            {
                QListWidgetItem* item = new QListWidgetItem;
                item->setText(digital->szDigitalName);
                item->setData(Qt::UserRole,digital->wDigitalID);
                ui->ptListWidget->insertItem(i,item);
            }
        }
    }
    else if(m_btPtType == 1)//yc
    {
        for(int i = 0;i < m_pUserHandle->m_analogueList.count();i++)
        {
            ANALOGUE *analogue = m_pUserHandle->m_analogueList[i];
            if(analogue && analogue->wStationID == wStationID && !m_pUserHandle->findPointInDb(m_btPtType,analogue->wStationID,analogue->wAnalogueID))
            {
                QListWidgetItem* item = new QListWidgetItem;
                item->setText(analogue->szAnalogueName);
                item->setData(Qt::UserRole,analogue->wAnalogueID);
                ui->ptListWidget->insertItem(i,item);
            }
        }
    }
}

void HPointInputDlg::addBtn_clicked()
{
    if(!m_pUserHandle)
        return;
    quint16 wStationID = ui->stComboBox->currentData().toUInt();
    QList<QListWidgetItem*> itemList = ui->ptListWidget->selectedItems();
    for(int i = 0;i < itemList.count();i++)
    {
        QListWidgetItem* item = itemList[i];
        if(!item) continue;
        quint16 wPointID = item->data(Qt::UserRole).toUInt();

        POINTITEM* pi = new POINTITEM;
        pi->wStationNo = wStationID;
        pi->wPointNo = wPointID;
        if(m_btPtType == 0)
            m_pUserHandle->piConfig.m_yxList.append(pi);
        else if(m_btPtType == 1)
            m_pUserHandle->piConfig.m_ycList.append(pi);
        else
        {
            delete pi;
            pi = NULL;
        }
    }

    //刷新左右两边
    stComboBox_changed();
}

void HPointInputDlg::addAllBtn_clicked()
{
    if(!m_pUserHandle)
        return;
    quint16 wStationID = ui->stComboBox->currentData().toUInt();
    ui->ptListWidget->selectAll();
    QList<QListWidgetItem*> itemList = ui->ptListWidget->selectedItems();
    for(int i = 0;i < itemList.count();i++)
    {
        QListWidgetItem* item = itemList[i];
        if(!item) continue;
        quint16 wPointID = item->data(Qt::UserRole).toUInt();

        POINTITEM* pi = new POINTITEM;
        pi->wStationNo = wStationID;
        pi->wPointNo = wPointID;
        if(m_btPtType == 0)
            m_pUserHandle->piConfig.m_yxList.append(pi);
        else if(m_btPtType == 1)
            m_pUserHandle->piConfig.m_ycList.append(pi);
        else
        {
            delete pi;
            pi = NULL;
        }
    }
    stComboBox_changed();
}

void HPointInputDlg::delBtn_clicked()
{
    if(!m_pUserHandle)
        return;
    //quint16 wStationID = ui->stComboBox->currentData().toUInt();
    QList<QListWidgetItem*> itemList = ui->sendPtlistWidget->selectedItems();
    for(int i = 0;i < itemList.count();i++)
    {
        QListWidgetItem* item = itemList[i];
        if(!item) continue;
        POINTITEM* pi = item->data(Qt::UserRole).value<POINTITEM*>();
        if(pi)
        {
            if(m_btPtType == 0)
                m_pUserHandle->piConfig.m_yxList.removeOne(pi);
            else if(m_btPtType == 1)
                m_pUserHandle->piConfig.m_ycList.removeOne(pi);
            delete pi;
            pi = NULL;
        }

    }

    //刷新左右两边
    stComboBox_changed();
}

void HPointInputDlg::delAllBtn_clicked()
{
    if(!m_pUserHandle)
        return;
    ui->sendPtlistWidget->selectAll();
    QList<QListWidgetItem*> itemList = ui->sendPtlistWidget->selectedItems();
    for(int i = 0;i < itemList.count();i++)
    {
        QListWidgetItem* item = itemList[i];
        if(!item) continue;
        POINTITEM* pi = item->data(Qt::UserRole).value<POINTITEM*>();
        if(pi)
        {
            if(m_btPtType == 0)
                m_pUserHandle->piConfig.m_yxList.removeOne(pi);
            else if(m_btPtType == 1)
                m_pUserHandle->piConfig.m_ycList.removeOne(pi);
            delete pi;
            pi = NULL;
        }
    }

    //刷新左右两边
    stComboBox_changed();
}
