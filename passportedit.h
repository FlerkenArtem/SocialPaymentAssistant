#ifndef PASSPORTEDIT_H
#define PASSPORTEDIT_H

#include <QWidget>
#include "passportdata.h"

namespace Ui {
class PassportEdit;
}

class PassportEdit : public QWidget
{
    Q_OBJECT

public:
    enum UserType { Applicant, Employee };

    explicit PassportEdit(UserType userType, QWidget *parent = nullptr);
    ~PassportEdit();

    PassportData getPassportData() const { return passportData; }
    bool isDataValid() const { return dataValid; }

signals:
    void passportDataCompleted(const PassportData &data);
    void passportEditCancelled();

private slots:
    void on_passportOK_clicked();
    void on_passportAddScan_clicked();

private:
    Ui::PassportEdit *ui;
    UserType userType;
    PassportData passportData;
    bool dataValid;

    bool validateData();
};

#endif // PASSPORTEDIT_H
