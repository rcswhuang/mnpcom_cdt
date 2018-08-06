#ifndef HPOINTSELDLG_H
#define HPOINTSELDLG_H

#include <QDialog>

namespace Ui {
class pointSelDlg;
}

class HUserHandle;
class HPointSelDlg : public QDialog
{
    Q_OBJECT

public:
    explicit HPointSelDlg(HUserHandle* userHandle,QWidget *parent = 0);
    ~HPointSelDlg();
    void init();
    void refreshPointList();
protected:
    void resizeEvent(QResizeEvent *event);
    void contextMenuEvent(QContextMenuEvent *e);

public slots:
    void yxcustomContextMenuRequested(const QPoint &pos);
    void yccustomContextMenuRequested(const QPoint &pos);
    void inputPoint();
private:
    HUserHandle* m_pUserHandle;
    quint16 m_wSendYxTime;
    quint16 m_wSendYcTime;
    QString m_strPortName;
    qint32 m_strBaudRate;

    quint8 m_btPtType;
private:
    Ui::pointSelDlg *ui;
};

#endif // HPOINTSELDLG_H
