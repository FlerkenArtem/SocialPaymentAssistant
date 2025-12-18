#include "empreg.h"
#include "empmainwindow.h"
#include "ui_empreg.h"
#include <QSqlError>
#include <QDate>

EmpReg::EmpReg(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EmpReg)
{
    ui->setupUi(this);

    // Настраиваем даты по умолчанию
    ui->passportDateOfIssue->setDate(QDate::currentDate());
    ui->birthDate->setDate(QDate::currentDate().addYears(-30));

    // Загружаем данные из БД
    loadBranches();
    loadPositions();
    loadPassportDepartments();
    loadDepartments();
}

EmpReg::~EmpReg()
{
    delete ui;
}

void EmpReg::loadBranches()
{
    ui->branchOffice->clear();
    ui->branchOffice->addItem("Выберите филиал", -1);

    QSqlQuery query;
    query.prepare("SELECT BRANCH_OFFICE_ID, BRANCH_NAME FROM BRANCH_OFFICE ORDER BY BRANCH_NAME");

    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString name = query.value(1).toString();
            ui->branchOffice->addItem(name, id);
        }
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось загрузить список филиалов:\n" + query.lastError().text());
    }
}

void EmpReg::loadPositions()
{
    ui->position->clear();
    ui->position->addItem("Выберите должность", -1);

    QSqlQuery query;
    query.prepare("SELECT EMPLOYEE_POSITION_ID, POSITION FROM EMPLOYEE_POSITION ORDER BY POSITION");

    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString position = query.value(1).toString();
            ui->position->addItem(position, id);
        }
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось загрузить список должностей:\n" + query.lastError().text());
    }
}

void EmpReg::loadPassportDepartments()
{
    ui->passportIssuingDepartment->clear();
    ui->passportIssuingDepartment->addItem("Введите отдел", -1);

    QSqlQuery query;
    query.prepare("SELECT PASSPORT_ISSUING_DEPARTMENT_ID, ISSUING_DEPARTMENT FROM PASSPORT_ISSUING_DEPARTMENT ORDER BY ISSUING_DEPARTMENT");

    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString department = query.value(1).toString();
            ui->passportIssuingDepartment->addItem(department, id);
        }
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось загрузить список отделов, выдавших паспорт:\n" + query.lastError().text());
    }
}

void EmpReg::loadDepartments(int branchId)
{
    ui->department->clear();

    QSqlQuery query;

    if (branchId == -1) {
        // Загружаем все отделы
        query.prepare("SELECT d.DEPARTMENT_ID, d.DEPARTMENT_NAME, b.BRANCH_NAME "
                      "FROM DEPARTMENT d "
                      "INNER JOIN BRANCH_OFFICE b ON d.BRANCH_OFFICE_ID = b.BRANCH_OFFICE_ID "
                      "ORDER BY b.BRANCH_NAME, d.DEPARTMENT_NAME");
    } else {
        // Загружаем отделы только для выбранного филиала
        query.prepare("SELECT DEPARTMENT_ID, DEPARTMENT_NAME FROM DEPARTMENT "
                      "WHERE BRANCH_OFFICE_ID = :branchId "
                      "ORDER BY DEPARTMENT_NAME");
        query.bindValue(":branchId", branchId);
    }

    if (query.exec()) {
        if (branchId == -1) {
            ui->department->addItem("Выберите отдел", -1);
            while (query.next()) {
                int id = query.value(0).toInt();
                QString name = query.value(1).toString();
                QString branch = query.value(2).toString();
                ui->department->addItem(name + " (" + branch + ")", id);
            }
        } else {
            ui->department->addItem("Выберите отдел", -1);
            while (query.next()) {
                int id = query.value(0).toInt();
                QString name = query.value(1).toString();
                ui->department->addItem(name, id);
            }
        }
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось загрузить список отделов:\n" + query.lastError().text());
    }
}

void EmpReg::on_branchOfficeFilterBtn_clicked()
{
    int branchId = ui->branchOffice->currentData().toInt();
    if (branchId == -1) {
        QMessageBox::warning(this, "Выбор филиала", "Пожалуйста, выберите филиал для фильтрации");
        return;
    }

    loadDepartments(branchId);
}

void EmpReg::on_checkBox_toggled(bool checked)
{
    ui->patronymic->setEnabled(!checked);
    if (checked) {
        ui->patronymic->clear();
    }
}

void EmpReg::on_empRegOK_clicked()
{
    // Проверка обязательных полей
    if (ui->login->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите логин");
        return;
    }

    if (ui->eMail->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите email");
        return;
    }

    if (ui->password->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите пароль");
        return;
    }

    if (ui->password->text() != ui->password2->text()) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают");
        return;
    }

    if (ui->password->text().length() < 8) {
        QMessageBox::warning(this, "Ошибка", "Пароль должен содержать минимум 8 символов");
        return;
    }

    if (ui->surname->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите фамилию");
        return;
    }

    if (ui->name->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите имя");
        return;
    }

    if (ui->phone->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите номер телефона");
        return;
    }

    // Паспортные данные
    if (ui->passportSeriesEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите серию паспорта");
        return;
    }

    if (ui->passportNumberEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите номер паспорта");
        return;
    }

    if (ui->passportIssuingDepartment->currentText().isEmpty() ||
        ui->passportIssuingDepartment->currentData().toInt() == -1) {
        QMessageBox::warning(this, "Ошибка", "Введите отдел, выдавший паспорт");
        return;
    }

    if (ui->passportDivisionCode->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите код подразделения");
        return;
    }

    // Трудовая книжка
    if (ui->workBookSeries->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите серию трудовой книжки");
        return;
    }

    if (ui->workBookNum->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите номер трудовой книжки");
        return;
    }

    // ИНН
    if (ui->inn->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите ИНН");
        return;
    }

    // Рабочие данные
    if (ui->branchOffice->currentData().toInt() == -1) {
        QMessageBox::warning(this, "Ошибка", "Выберите филиал");
        return;
    }

    if (ui->department->currentData().toInt() == -1) {
        QMessageBox::warning(this, "Ошибка", "Выберите отдел");
        return;
    }

    if (ui->position->currentData().toInt() == -1) {
        QMessageBox::warning(this, "Ошибка", "Выберите должность");
        return;
    }

    // Подготавливаем данные для процедуры
    QString username = ui->login->text();
    QString password = ui->password->text();
    QString email = ui->eMail->text();
    QString surname = ui->surname->text();
    QString name = ui->name->text();
    QString patronymic = ui->checkBox->isChecked() ? "" : ui->patronymic->text();
    QString phoneNumber = ui->phone->text();
    QString inn = ui->inn->text();
    QString positionText = ui->position->currentText();
    QString departmentName = ui->department->currentText();
    QString passportSeries = ui->passportSeriesEdit->text();
    QString passportNumber = ui->passportNumberEdit->text();
    QString passportIssuedBy = ui->passportIssuingDepartment->currentText();
    QString passportIssueDate = ui->passportDateOfIssue->date().toString("yyyy-MM-dd");
    QString passportDivisionCode = ui->passportDivisionCode->text();
    QString birthDate = ui->birthDate->date().toString("yyyy-MM-dd");
    QString workBookSeries = ui->workBookSeries->text();
    QString workBookNumber = ui->workBookNum->text();

    // Вызов хранимой процедуры
    QSqlQuery query;
    query.prepare("call new_emp(:username, :password, :email, "
                  ":surname, :name, :patronymic, :phoneNumber, "
                  ":inn, :positionText, :departmentName, "
                  ":passportSeries, :passportNumber, "
                  ":passportIssuedBy, :passportIssueDate, "
                  ":passportDivisionCode, :birthDate, "
                  ":workBookSeries, :workBookNumber);");

    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":email", email);
    query.bindValue(":surname", surname);
    query.bindValue(":name", name);
    query.bindValue(":patronymic", patronymic);
    query.bindValue(":phoneNumber", phoneNumber);
    query.bindValue(":inn", inn);
    query.bindValue(":positionText", positionText);
    query.bindValue(":departmentName", departmentName);
    query.bindValue(":passportSeries", passportSeries);
    query.bindValue(":passportNumber", passportNumber);
    query.bindValue(":passportIssuedBy", passportIssuedBy);
    query.bindValue(":passportIssueDate", passportIssueDate);
    query.bindValue(":passportDivisionCode", passportDivisionCode);
    query.bindValue(":birthDate", birthDate);
    query.bindValue(":workBookSeries", workBookSeries);
    query.bindValue(":workBookNumber", workBookNumber);

    if (query.exec()) {
        QMessageBox::information(this, "Успех", "Сотрудник успешно зарегистрирован!");

        // Получаем ID только что созданного сотрудника
        QSqlQuery idQuery;
        idQuery.prepare(
            "SELECT e.employee_id "
            "FROM employee e "
            "JOIN account a ON e.account_id = a.account_id "
            "WHERE a.login = :username"
            );
        idQuery.bindValue(":username", username);

        if (idQuery.exec() && idQuery.next()) {
            int employeeId = idQuery.value(0).toInt();
            EmpMainWindow *empWindow = new EmpMainWindow(employeeId);
            empWindow->show();
            this->close();
        }
    } else {
        QMessageBox::critical(this, "Ошибка регистрации",
                              "Не удалось зарегистрировать сотрудника:\n" +
                                  query.lastError().text());
    }
}

