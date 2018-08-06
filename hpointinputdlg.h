#ifndef HPOINTINPUTDLG_H
#define HPOINTINPUTDLG_H

#include <QDialog>

namespace Ui {
class pointInputDlg;
}
class HUserHandle;
class HPointInputDlg : public QDialog
{
    Q_OBJECT

public:
    explicit HPointInputDlg(quint8 type,HUserHandle* handle,QWidget *parent = 0);
    ~HPointInputDlg();
public:
    void init();
    void initPoint();
public slots:
    void stComboBox_changed();
    void addBtn_clicked();
    void addAllBtn_clicked();
    void delBtn_clicked();
    void delAllBtn_clicked();
private:
    Ui::pointInputDlg *ui;
    HUserHandle* m_pUserHandle;
    quint8 m_btPtType;
};

#endif // HPOINTINPUTDLG_H
