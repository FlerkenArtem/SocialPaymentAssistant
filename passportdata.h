#ifndef PASSPORTDATA_H
#define PASSPORTDATA_H

#include <QString>
#include <QDate>

struct PassportData {
    QString series;
    QString number;
    QDate birthDate;
    QString issuingDepartment;
    QDate dateOfIssue;
    QString divisionCode;
    QString scanPath;
    bool isEmpty;

    PassportData() : isEmpty(true) {}

    bool isValidForApplicant() const {
        return !series.isEmpty() && !number.isEmpty() &&
               birthDate.isValid() && !issuingDepartment.isEmpty() &&
               dateOfIssue.isValid() && !divisionCode.isEmpty() &&
               !scanPath.isEmpty();
    }

    bool isValidForEmployee() const {
        return !series.isEmpty() && !number.isEmpty() &&
               birthDate.isValid() && !issuingDepartment.isEmpty() &&
               dateOfIssue.isValid() && !divisionCode.isEmpty();
        // scanPath не проверяем - для сотрудника скан не требуется
    }
};

#endif // PASSPORTDATA_H
