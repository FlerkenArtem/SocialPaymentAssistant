#include "empdatainsert.h"
#include "ui_empdatainsert.h"
#include "passportedit.h"
#include "empmainwindow.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

EmpDataInsert::EmpDataInsert(const QString &login, const QString &email,
                             const QString &password, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EmpDataInsert)
    , userLogin(login)
    , userEmail(email)
    , userPassword(password)
{
    ui->setupUi(this);

    // Настраиваем начальные значения
    ui->empNoPatronymicCheck->setChecked(false);

    // Подключаем сигналы и слоты
    connect(ui->empPassportButton, &QPushButton::clicked,
            this, &EmpDataInsert::on_empPassportButton_clicked);
    connect(ui->empNoPatronymicCheck, &QCheckBox::stateChanged,
            this, &EmpDataInsert::on_empNoPatronymicCheck_stateChanged);
    connect(ui->empDataOK, &QPushButton::clicked,
            this, &EmpDataInsert::on_empDataOK_clicked);

    // Загружаем данные из БД
    loadPositionsFromDB();
    loadBranchOfficesFromDB();
}

EmpDataInsert::~EmpDataInsert()
{
    delete ui;
}

void EmpDataInsert::on_empPassportButton_clicked()
{
    PassportEdit *passportEdit = new PassportEdit(PassportEdit::Employee);

    // Настраиваем окно как отдельное
    passportEdit->setWindowModality(Qt::WindowModal);
    passportEdit->setAttribute(Qt::WA_DeleteOnClose);

    // Подключаем сигнал завершения ввода паспортных данных
    connect(passportEdit, &PassportEdit::passportDataCompleted,
            this, &EmpDataInsert::onPassportDataReceived);

    // Подключаем сигнал отмены (если окно закрыли крестиком)
    connect(passportEdit, &PassportEdit::passportEditCancelled,
            passportEdit, &QWidget::deleteLater);

    passportEdit->show();
}

void EmpDataInsert::onPassportDataReceived(const PassportData &data)
{
    passportData = data;

    if (passportData.isValidForEmployee()) {
        ui->empPassportButton->setText("Паспорт заполнен ✓");
        ui->empPassportButton->setStyleSheet("color: green;");
    } else {
        ui->empPassportButton->setText("Паспорт не заполнен");
        ui->empPassportButton->setStyleSheet("color: red;");
    }
}

void EmpDataInsert::on_empNoPatronymicCheck_stateChanged(int state)
{
    if (state == Qt::Checked) {
        ui->empPatronymicEdit->setEnabled(false);
        ui->empPatronymicEdit->clear();
    } else {
        ui->empPatronymicEdit->setEnabled(true);
    }
}

void EmpDataInsert::loadPositionsFromDB()
{
    ui->empPositionComboBox->clear();

    QSqlQuery query;
    query.prepare("SELECT position FROM employee_position ORDER BY position");

    if (query.exec()) {
        while (query.next()) {
            ui->empPositionComboBox->addItem(query.value(0).toString());
        }
    } else {
        qDebug() << "Ошибка загрузки должностей:" << query.lastError().text();
    }
}

void EmpDataInsert::loadBranchOfficesFromDB()
{
    ui->empBranchOfficeComboBox->clear();
    ui->empBranchOfficeComboBox->addItem("Выберите филиал", -1);

    QSqlQuery query;
    query.prepare("SELECT branch_office_id, branch_office_name FROM branch_office ORDER BY branch_office_name");

    if (query.exec()) {
        while (query.next()) {
            ui->empBranchOfficeComboBox->addItem(
                query.value(1).toString(),
                query.value(0).toInt()
                );
        }

        // Подключаем сигнал изменения выбора филиала
        connect(ui->empBranchOfficeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this](int index) {
                    if (index > 0) {
                        int branchOfficeId = ui->empBranchOfficeComboBox->currentData().toInt();
                        loadDepartmentsFromDB(branchOfficeId);
                    } else {
                        ui->empDepartmentComboBox->clear();
                        ui->empDepartmentComboBox->addItem("Выберите отдел", -1);
                    }
                });
    } else {
        qDebug() << "Ошибка загрузки филиалов:" << query.lastError().text();
    }
}

void EmpDataInsert::loadDepartmentsFromDB(int branchOfficeId)
{
    ui->empDepartmentComboBox->clear();
    ui->empDepartmentComboBox->addItem("Выберите отдел", -1);

    if (branchOfficeId <= 0) return;

    QSqlQuery query;
    query.prepare("SELECT department_id, department_name FROM department "
                  "WHERE branch_office_id = :branch_office_id "
                  "ORDER BY department_name");
    query.bindValue(":branch_office_id", branchOfficeId);

    if (query.exec()) {
        while (query.next()) {
            ui->empDepartmentComboBox->addItem(
                query.value(1).toString(),
                query.value(0).toInt()
                );
        }
    } else {
        qDebug() << "Ошибка загрузки отделов:" << query.lastError().text();
    }
}

bool EmpDataInsert::validateInputData()
{
    // Проверка ФИО
    if (ui->empSurnameEdit->text().trimmed().isEmpty()) {
        showErrorMessage("Введите фамилию");
        return false;
    }

    if (ui->empNameEdit->text().trimmed().isEmpty()) {
        showErrorMessage("Введите имя");
        return false;
    }

    if (!ui->empNoPatronymicCheck->isChecked() &&
        ui->empPatronymicEdit->text().trimmed().isEmpty()) {
        showErrorMessage("Введите отчество или отметьте 'Нет отчества'");
        return false;
    }

    // Проверка телефона
    QString phone = ui->empPhoneEdit->text().trimmed();
    if (phone.isEmpty()) {
        showErrorMessage("Введите номер телефона");
        return false;
    }

    // Проверка ИНН
    QString inn = ui->empInnEdit->text().trimmed();
    if (inn.isEmpty()) {
        showErrorMessage("Введите ИНН");
        return false;
    }
    if (inn.length() != 12 || !inn.toLongLong()) {
        showErrorMessage("ИНН должен состоять из 12 цифр");
        return false;
    }

    // Проверка паспортных данных
    if (!passportData.isValidForEmployee()) {
        showErrorMessage("Заполните паспортные данные");
        return false;
    }

    // Проверка трудовой книжки
    QString workSeries = ui->empWorkRecordBookSeriesEdit->text().trimmed();
    QString workNumber = ui->empWorkRecordBookNumberEdit->text().trimmed();
    if (workSeries.isEmpty() || workNumber.isEmpty()) {
        showErrorMessage("Заполните данные трудовой книжки");
        return false;
    }

    // Проверка должности
    if (ui->empPositionComboBox->currentIndex() < 0 ||
        ui->empPositionComboBox->currentText().isEmpty()) {
        showErrorMessage("Выберите должность");
        return false;
    }

    // Проверка филиала
    if (ui->empBranchOfficeComboBox->currentIndex() <= 0) {
        showErrorMessage("Выберите филиал");
        return false;
    }

    // Проверка отдела
    if (ui->empDepartmentComboBox->currentIndex() <= 0) {
        showErrorMessage("Выберите отдел");
        return false;
    }

    return true;
}

void EmpDataInsert::showErrorMessage(const QString &message)
{
    QMessageBox::warning(this, "Ошибка заполнения", message);
}

bool EmpDataInsert::isDataValid() const
{
    // Упрощенная проверка для внешних вызовов
    return !ui->empSurnameEdit->text().isEmpty() &&
           !ui->empNameEdit->text().isEmpty() &&
           passportData.isValidForEmployee();
}

void EmpDataInsert::on_empDataOK_clicked()
{
    if (!validateInputData()) {
        return;
    }

    // Подготовка данных для процедуры
    QString surname = ui->empSurnameEdit->text().trimmed();
    QString name = ui->empNameEdit->text().trimmed();
    QString patronymic = ui->empNoPatronymicCheck->isChecked() ?
                             QString() : ui->empPatronymicEdit->text().trimmed();
    QString phone = ui->empPhoneEdit->text().trimmed();
    QString inn = ui->empInnEdit->text().trimmed();
    QString workSeries = ui->empWorkRecordBookSeriesEdit->text().trimmed();
    QString workNumber = ui->empWorkRecordBookNumberEdit->text().trimmed();
    QString position = ui->empPositionComboBox->currentText();
    QString departmentName = ui->empDepartmentComboBox->currentText();

    // Вызов хранимой процедуры new_emp
    QSqlQuery query;
    query.prepare("CALL new_emp(:username, :password, :email, :surname, :name, "
                  ":patronymic, :phone, :inn, :position, :department, "
                  ":passport_series, :passport_number, :passport_issued_by, "
                  ":passport_issue_date, :passport_division_code, :birth_date, "
                  ":work_series, :work_number)");

    query.bindValue(":username", userLogin);
    query.bindValue(":password", userPassword);
    query.bindValue(":email", userEmail);
    query.bindValue(":surname", surname);
    query.bindValue(":name", name);
    query.bindValue(":patronymic", patronymic.isEmpty() ? QString() : patronymic);
    query.bindValue(":phone", phone);
    query.bindValue(":inn", inn);
    query.bindValue(":position", position);
    query.bindValue(":department", departmentName);
    query.bindValue(":passport_series", passportData.series);
    query.bindValue(":passport_number", passportData.number);
    query.bindValue(":passport_issued_by", passportData.issuingDepartment);
    query.bindValue(":passport_issue_date", passportData.dateOfIssue);
    query.bindValue(":passport_division_code", passportData.divisionCode);
    query.bindValue(":birth_date", passportData.birthDate);
    query.bindValue(":work_series", workSeries);
    query.bindValue(":work_number", workNumber);

    if (query.exec()) {
        QMessageBox::information(this, "Успех",
                                 "Регистрация сотрудника завершена успешно!");

        // Переход к главному окну сотрудника
        EmpMainWindow *empMainWindow = new EmpMainWindow();
        empMainWindow->show();
        this->close();
    } else {
        QString errorText = query.lastError().text();
        QMessageBox::critical(this, "Ошибка регистрации",
                              "Не удалось зарегистрировать сотрудника.\n"
                              "Ошибка: " + errorText);
        qDebug() << "Ошибка выполнения процедуры new_emp:" << errorText;
    }
}
