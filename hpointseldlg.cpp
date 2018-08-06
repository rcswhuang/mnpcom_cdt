#include "hpointseldlg.h"
#include "ui_pointseldlg.h"
#include "huserhandle.h"
#include <QSerialPortInfo>
#include <QMenu>
#include <QHeaderView>
#include "hpointinputdlg.h"
HPointSelDlg::HPointSelDlg(HUserHandle * userhandle,QWidget *parent) :
    m_pUserHandle(userhandle),QDialog(parent),
    ui(new Ui::pointSelDlg)
{
    ui->setupUi(this);
    m_btPtType = 0;
    init();

}

HPointSelDlg::~HPointSelDlg()
{
    delete ui;
}

void HPointSelDlg::init()
{
    //初始化
    ui->yxTableWidget->setContextMenuPolicy (Qt::CustomContextMenu);
    ui->ycTableWidget->setContextMenuPolicy (Qt::CustomContextMenu);
    connect(ui->yxTableWidget,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(yxcustomContextMenuRequested(const QPoint &)));
    connect(ui->ycTableWidget,SIGNAL(customContextMenuRequested(const QPoint &)),this,SLOT(yccustomContextMenuRequested(const QPoint &)));
    //初始化界面
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidget);
    item->setText(0,QStringLiteral("插件名称"));
    ui->treeWidget->insertTopLevelItem(0,item);
    QTreeWidgetItem* pluginItem = new QTreeWidgetItem(item);
    pluginItem->setText(0,"Cdt");
    item->addChild(pluginItem);
    ui->treeWidget->expandAll();

    ui->yxTableWidget->setColumnWidth(0,100);
    ui->yxTableWidget->setColumnWidth(1,200);

    ui->ycTableWidget->setColumnWidth(0,100);
    ui->ycTableWidget->setColumnWidth(1,200);


    ui->yxCycleEdit->setText("30");
    ui->ycCycleEdit->setText("30");

    QList<QSerialPortInfo> spInfoList = QSerialPortInfo::availablePorts();
    for(int i = 0;i < spInfoList.count();i++)
        ui->serialPortComboBox->addItem(spInfoList[i].portName());

    QList<qint32> baudRatesList = QSerialPortInfo::standardBaudRates();
    for(int i = 0;i < baudRatesList.count();i++)
    {
        QString strBaudRate = QString("%1,n,8,1").arg(baudRatesList[i]);
        ui->baudComboBox->addItem(strBaudRate,baudRatesList[i]);
    }


    if(m_pUserHandle)
    {
        m_wSendYxTime = m_pUserHandle->piConfig.m_wSendYxTime;
        ui->yxCycleEdit->setText(QString("%1").arg(m_wSendYxTime));
        m_wSendYcTime = m_pUserHandle->piConfig.m_wSendYcTime;
        ui->ycCycleEdit->setText(QString("%1").arg(m_wSendYcTime));

        int nIndex = ui->serialPortComboBox->findText(m_pUserHandle->piConfig.m_strPortName);
        if(nIndex != (int)-1)
            ui->serialPortComboBox->setCurrentIndex(nIndex);
        else
            ui->serialPortComboBox->setCurrentIndex(0);

        nIndex = ui->baudComboBox->findData(m_pUserHandle->piConfig.m_nPortBaudRate);
        if(nIndex != (int)-1)
            ui->baudComboBox->setCurrentIndex(nIndex);
        else
            ui->baudComboBox->setCurrentIndex(0);

        refreshPointList();
    }
}

void HPointSelDlg::refreshPointList()
{
    if(m_btPtType == 0)
    {
        ui->yxTableWidget->clear();
        ui->yxTableWidget->setColumnCount(2);
        QTableWidgetItem *qtablewidgetitem = new QTableWidgetItem;
        qtablewidgetitem->setText(QStringLiteral("点号"));
        ui->yxTableWidget->setHorizontalHeaderItem(0,qtablewidgetitem);
        QTableWidgetItem *qtablewidgetitem1 = new QTableWidgetItem;
        qtablewidgetitem1->setText(QStringLiteral("遥信测点"));
        ui->yxTableWidget->setHorizontalHeaderItem(1,qtablewidgetitem1);
        ui->yxTableWidget->setRowCount(m_pUserHandle->piConfig.m_yxList.count());
        for(int i = 0; i < m_pUserHandle->piConfig.m_yxList.count();i++)
        {
            POINTITEM* pi = m_pUserHandle->piConfig.m_yxList[i];
            QVariant var;
            var.setValue(pi);
            QTableWidgetItem* item = new QTableWidgetItem;
            item->setText(QString("%1").arg(i+1));
            item->setData(Qt::UserRole,var);
            ui->yxTableWidget->setItem(i,0,item);
            QTableWidgetItem* item1 = new QTableWidgetItem;
            item1->setText(m_pUserHandle->getPointName(m_btPtType,pi->wStationNo,pi->wPointNo));
            item1->setData(Qt::UserRole,var);
            ui->yxTableWidget->setItem(i,1,item1);
        }
    }
    else if(m_btPtType == 1)
    {
        ui->ycTableWidget->clear();
        ui->ycTableWidget->setColumnCount(2);
        QTableWidgetItem *qtablewidgetitem2 = new QTableWidgetItem;
        qtablewidgetitem2->setText(QStringLiteral("点号"));
        ui->ycTableWidget->setHorizontalHeaderItem(0,qtablewidgetitem2);
        QTableWidgetItem *qtablewidgetitem3 = new QTableWidgetItem;
        qtablewidgetitem3->setText(QStringLiteral("遥信测点"));
        ui->ycTableWidget->setHorizontalHeaderItem(1,qtablewidgetitem3);
        ui->ycTableWidget->setRowCount(m_pUserHandle->piConfig.m_ycList.count());
        for(int i = 0; i < m_pUserHandle->piConfig.m_ycList.count();i++)
        {
            POINTITEM* pi = m_pUserHandle->piConfig.m_ycList[i];
            QVariant var;
            var.setValue(pi);
            QTableWidgetItem* item = new QTableWidgetItem;
            item->setText(QString("%1").arg(i+1));
            item->setData(Qt::UserRole,var);
            ui->ycTableWidget->setItem(i,0,item);
            QTableWidgetItem* item1 = new QTableWidgetItem;
            item1->setText(m_pUserHandle->getPointName(m_btPtType,pi->wStationNo,pi->wPointNo));
            item1->setData(Qt::UserRole,var);
            ui->ycTableWidget->setItem(i,1,item1);
        }
    }
}

void HPointSelDlg::resizeEvent(QResizeEvent *event)
{
    int width = ui->transTabWidget->width();
    ui->yxTableWidget->setColumnWidth(0,width/5);
    ui->yxTableWidget->setColumnWidth(1,width*4/5 - 50);

    ui->ycTableWidget->setColumnWidth(0,width/5);
    ui->ycTableWidget->setColumnWidth(1,width*4/5 - 50);
    //QDialog::resizeEvent(event);
}

void HPointSelDlg::contextMenuEvent(QContextMenuEvent *e)
{

}

void HPointSelDlg::yxcustomContextMenuRequested(const QPoint &pos)
{
    QMenu* menu = new QMenu;
    QAction *oneAct = new QAction(QStringLiteral("导入遥信"),this);
    connect(oneAct,SIGNAL(triggered(bool)),this,SLOT(inputPoint()));
    m_btPtType = 0;
    menu->addAction(oneAct);
    menu->popup(QCursor::pos());
}

void HPointSelDlg::yccustomContextMenuRequested(const QPoint &pos)
{
    m_btPtType = 1;
    QMenu* menu = new QMenu;
    QAction *oneAct = new QAction(QStringLiteral("导入遥测"),this);
    connect(oneAct,SIGNAL(triggered(bool)),this,SLOT(inputPoint()));
    menu->addAction(oneAct);
    menu->popup(QCursor::pos());
}


void HPointSelDlg::inputPoint()
{
   HPointInputDlg dlg(m_btPtType,m_pUserHandle);
   if(QDialog::Accepted == dlg.exec())
       refreshPointList();
}
