#include "passportedit.h"
#include "ui_passportedit.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

PassportEdit::PassportEdit(UserType userType, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PassportEdit)
    , userType(userType)
    , dataValid(false)
{
    ui->setupUi(this);

    // Настраиваем окно как отдельное
    this->setWindowFlags(Qt::Window);

    // Настраиваем интерфейс в зависимости от типа пользователя
    if (userType == Employee) {
        ui->passportAddScan->setEnabled(false);
        ui->passportAddScan->setVisible(false); // Скрываем кнопку скана для сотрудника
        this->setWindowTitle("Паспортные данные (Сотрудник)");
    } else {
        this->setWindowTitle("Паспортные данные (Заявитель)");
    }

    // Устанавливаем даты по умолчанию
    ui->birthDate->setDate(QDate::currentDate().addYears(-18));
    ui->passportDateOfIssue->setDate(QDate::currentDate());

    // Подключаем слоты
    connect(ui->passportOK, &QPushButton::clicked, this, &PassportEdit::on_passportOK_clicked);
    connect(ui->passportAddScan, &QPushButton::clicked, this, &PassportEdit::on_passportAddScan_clicked);

    // Подключаем сигнал закрытия окна
    connect(this, &QWidget::destroyed, [this]() {
        if (!dataValid) {
            emit passportEditCancelled();
        }
    });
}

PassportEdit::~PassportEdit()
{
    delete ui;
}

void PassportEdit::on_passportAddScan_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Выберите скан паспорта",
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    "Изображения (*.png *.jpg *.jpeg *.bmp);;Все файлы (*.*)");

    if (!fileName.isEmpty()) {
        passportData.scanPath = fileName;
        ui->passportAddScan->setText("Скан выбран");
        ui->passportAddScan->setStyleSheet("color: green;");
    }
}

bool PassportEdit::validateData()
{
    // Собираем данные
    passportData.series = ui->passportSeriesEdit->text().trimmed();
    passportData.number = ui->passportNumberEdit->text().trimmed();
    passportData.birthDate = ui->birthDate->date();
    passportData.issuingDepartment = ui->passportIssuingDepartment->currentText().trimmed();
    passportData.dateOfIssue = ui->passportDateOfIssue->date();
    passportData.divisionCode = ui->passportDivisionCode->text().trimmed();
    passportData.isEmpty = false;

    // Валидация в зависимости от типа пользователя
    if (userType == Applicant) {
        if (!passportData.isValidForApplicant()) {
            QStringList missingFields;

            if (passportData.series.isEmpty()) missingFields << "серия паспорта";
            if (passportData.number.isEmpty()) missingFields << "номер паспорта";
            if (!passportData.birthDate.isValid()) missingFields << "дата рождения";
            if (passportData.issuingDepartment.isEmpty()) missingFields << "кем выдан";
            if (!passportData.dateOfIssue.isValid()) missingFields << "дата выдачи";
            if (passportData.divisionCode.isEmpty()) missingFields << "код подразделения";
            if (passportData.scanPath.isEmpty()) missingFields << "скан паспорта";

            QMessageBox::warning(this, "Ошибка заполнения",
                                 "Для заявителя все поля паспорта обязательны.\n"
                                 "Не заполнены: " + missingFields.join(", "));
            return false;
        }

    } else { // Employee
        if (!passportData.isValidForEmployee()) {
            QStringList missingFields;

            if (passportData.series.isEmpty()) missingFields << "серия паспорта";
            if (passportData.number.isEmpty()) missingFields << "номер паспорта";
            if (!passportData.birthDate.isValid()) missingFields << "дата рождения";
            if (passportData.issuingDepartment.isEmpty()) missingFields << "кем выдан";
            if (!passportData.dateOfIssue.isValid()) missingFields << "дата выдачи";
            if (passportData.divisionCode.isEmpty()) missingFields << "код подразделения";

            QMessageBox::warning(this, "Ошибка заполнения",
                                 "Для сотрудника все паспортные данные обязательны.\n"
                                 "Не заполнены: " + missingFields.join(", "));
            return false;
        }
        // Для сотрудника скан не обязателен
    }

    // Общие проверки формата данных (если поля заполнены)
    if (!passportData.series.isEmpty() &&
        (passportData.series.length() != 4 || !passportData.series.toInt())) {
        QMessageBox::warning(this, "Ошибка", "Серия паспорта должна состоять из 4 цифр");
        return false;
    }

    if (!passportData.number.isEmpty() &&
        (passportData.number.length() != 6 || !passportData.number.toInt())) {
        QMessageBox::warning(this, "Ошибка", "Номер паспорта должен состоять из 6 цифр");
        return false;
    }

    if (!passportData.divisionCode.isEmpty() &&
        (passportData.divisionCode.length() != 7 || !passportData.divisionCode.toInt())) {
        QMessageBox::warning(this, "Ошибка", "Код подразделения должен состоять из 7 цифр");
        return false;
    }

    // Проверка логических условий
    if (passportData.dateOfIssue.isValid() && passportData.dateOfIssue > QDate::currentDate()) {
        QMessageBox::warning(this, "Ошибка", "Дата выдачи не может быть в будущем");
        return false;
    }

    if (passportData.birthDate.isValid() && passportData.birthDate > QDate::currentDate()) {
        QMessageBox::warning(this, "Ошибка", "Дата рождения не может быть в будущем");
        return false;
    }

    if (passportData.dateOfIssue.isValid() && passportData.birthDate.isValid() &&
        passportData.dateOfIssue < passportData.birthDate.addYears(14)) {
        QMessageBox::warning(this, "Ошибка",
                             "Паспорт не мог быть выдан ранее 14 лет");
        return false;
    }

    return true;
}

void PassportEdit::on_passportOK_clicked()
{
    if (!validateData()) {
        return;
    }

    dataValid = true;
    emit passportDataCompleted(passportData);
    this->close();
}
