#ifndef EMPREG_H
#define EMPREG_H

#include <QWidget>
#include <QSqlQuery>
#include <QMessageBox>

namespace Ui {
class EmpReg;
}

class EmpReg : public QWidget
{
    Q_OBJECT

public:
    explicit EmpReg(QWidget *parent = nullptr);
    ~EmpReg();

private slots:
    void on_empRegOK_clicked();
    void on_branchOfficeFilterBtn_clicked();
    void loadDepartments(int branchId = -1);
    void on_checkBox_toggled(bool checked);

private:
    Ui::EmpReg *ui;
    void loadBranches();
    void loadPositions();
    void loadPassportDepartments();
};

#endif // EMPREG_H
