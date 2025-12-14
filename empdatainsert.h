#ifndef EMPDATAINSERT_H
#define EMPDATAINSERT_H

#include <QWidget>
#include "passportdata.h"

namespace Ui {
class EmpDataInsert;
}

class EmpDataInsert : public QWidget
{
    Q_OBJECT

public:
    explicit EmpDataInsert(const QString &login, const QString &email,
                           const QString &password, QWidget *parent = nullptr);
    ~EmpDataInsert();

    PassportData getPassportData() const { return passportData; }
    bool hasValidPassport() const { return passportData.isValidForEmployee(); }
    bool isDataValid() const;

private slots:
    void on_empPassportButton_clicked();
    void on_empNoPatronymicCheck_stateChanged(int state);
    void on_empDataOK_clicked();
    void onPassportDataReceived(const PassportData &data);

private:
    Ui::EmpDataInsert *ui;
    PassportData passportData;
    QString userLogin;
    QString userEmail;
    QString userPassword;

    void loadPositionsFromDB();
    void loadBranchOfficesFromDB();
    void loadDepartmentsFromDB(int branchOfficeId = -1);
    bool validateInputData();
    void showErrorMessage(const QString &message);
};

#endif // EMPDATAINSERT_H
