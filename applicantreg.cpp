#include "applicantreg.h"
#include "ui_applicantreg.h"
#include "mainwindow.h"
#include <QSqlError>
#include <QDate>
#include <QRegularExpression>
#include <QDebug>

ApplicantReg::ApplicantReg(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ApplicantReg)
{
    ui->setupUi(this);

    // Настраиваем даты по умолчанию
    ui->passportDateOfIssue->setDate(QDate::currentDate());
    ui->birthDate->setDate(QDate::currentDate().addYears(-30));
    ui->openningDate->setDate(QDate::currentDate());

    // Загружаем данные из БД
    loadRegions();
    loadDistricts();
    loadCities();
    loadAreas();
    loadStreets();
    loadHouses();
    loadFlats();
    loadBanks();
    loadPassportDepartments();

    // Настраиваем комбобокс банка для ручного ввода
    ui->bankName->setEditable(true);

    // Инициализируем данные сканов как пустые
    passportScanData = QByteArray();
    snilsScanData = QByteArray();
    policyScanData = QByteArray();
    adoptionCertificateScanData = QByteArray();

    // Инициализация состояний
    on_noPatronymicCheck_toggled(ui->noPatronymicCheck->isChecked());
    on_noActualAddress_toggled(ui->noActualAddress->isChecked());
    on_noAdoptationCertificate_toggled(ui->noAdoptationCertificate->isChecked());
}

ApplicantReg::~ApplicantReg()
{
    delete ui;
}

void ApplicantReg::loadRegions()
{
    // Очищаем и загружаем регионы для обоих адресов
    ui->regionComboBoxRegAddress->clear();
    ui->regionComboBoxActualAddress->clear();

    ui->regionComboBoxRegAddress->addItem("Выберите регион", -1);
    ui->regionComboBoxActualAddress->addItem("Выберите регион", -1);

    QSqlQuery query;
    query.prepare("SELECT region_id, name_region FROM region ORDER BY name_region");

    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString name = query.value(1).toString();
            ui->regionComboBoxRegAddress->addItem(name, id);
            ui->regionComboBoxActualAddress->addItem(name, id);
        }
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось загрузить список регионов:\n" + query.lastError().text());
    }
}

void ApplicantReg::loadDistricts()
{
    // Загружаем муниципальные районы
    ui->districtComboBoxRegAddress->clear();
    ui->districtComboBoxActualAddress->clear();

    QSqlQuery query;
    query.prepare("SELECT district_id, name_district FROM district ORDER BY name_district");

    if (query.exec()) {
        while (query.next()) {
            QString name = query.value(1).toString();
            ui->districtComboBoxRegAddress->addItem(name);
            ui->districtComboBoxActualAddress->addItem(name);
        }
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось загрузить список муниципальных районов:\n" + query.lastError().text());
    }
}

void ApplicantReg::loadCities()
{
    // Загружаем города
    ui->cityComboBoxRegAddress->clear();
    ui->cityComboBoxActualAddress->clear();

    QSqlQuery query;
    query.prepare("SELECT city_id, name_city FROM city ORDER BY name_city");

    if (query.exec()) {
        while (query.next()) {
            QString name = query.value(1).toString();
            ui->cityComboBoxRegAddress->addItem(name);
            ui->cityComboBoxActualAddress->addItem(name);
        }
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось загрузить список городов:\n" + query.lastError().text());
    }
}

void ApplicantReg::loadAreas()
{
    // Загружаем районы городов
    ui->areaComboBoxRegAddress->clear();
    ui->areaComboBoxActualAddress->clear();

    QSqlQuery query;
    query.prepare("SELECT area_id, name_area FROM area ORDER BY name_area");

    if (query.exec()) {
        while (query.next()) {
            QString name = query.value(1).toString();
            ui->areaComboBoxRegAddress->addItem(name);
            ui->areaComboBoxActualAddress->addItem(name);
        }
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось загрузить список районов городов:\n" + query.lastError().text());
    }
}

void ApplicantReg::loadStreets()
{
    // Загружаем улицы
    ui->StreetComboBoxRegAddress->clear();
    ui->StreetComboBoxActualAddress->clear();

    QSqlQuery query;
    query.prepare("SELECT street_id, name_street FROM street ORDER BY name_street");

    if (query.exec()) {
        while (query.next()) {
            QString name = query.value(1).toString();
            ui->StreetComboBoxRegAddress->addItem(name);
            ui->StreetComboBoxActualAddress->addItem(name);
        }
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось загрузить список улиц:\n" + query.lastError().text());
    }
}

void ApplicantReg::loadHouses()
{
    // Загружаем дома
    ui->houseComboBoxRegAddress->clear();
    ui->houseComboBoxActualAddress->clear();

    QSqlQuery query;
    query.prepare("SELECT house_id, name_house FROM house ORDER BY name_house");

    if (query.exec()) {
        while (query.next()) {
            QString name = query.value(1).toString();
            ui->houseComboBoxRegAddress->addItem(name);
            ui->houseComboBoxActualAddress->addItem(name);
        }
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось загрузить список домов:\n" + query.lastError().text());
    }
}

void ApplicantReg::loadFlats()
{
    // Загружаем квартиры
    ui->flatComboBoxRegAddress->clear();
    ui->flatComboBoxActualAddress->clear();

    QSqlQuery query;
    query.prepare("SELECT flat_id, name_flat FROM flat ORDER BY name_flat");

    if (query.exec()) {
        while (query.next()) {
            QString name = query.value(1).toString();
            ui->flatComboBoxRegAddress->addItem(name);
            ui->flatComboBoxActualAddress->addItem(name);
        }
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось загрузить список квартир:\n" + query.lastError().text());
    }
}

void ApplicantReg::loadBanks()
{
    ui->bankName->clear();
    ui->bankName->addItem("Выберите банк или введите вручную", -1);

    QSqlQuery query;
    query.prepare("SELECT name_of_the_recipient_bank_id, name_of_the_bank FROM name_of_the_recipient_bank ORDER BY name_of_the_bank");

    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString name = query.value(1).toString();
            ui->bankName->addItem(name, id);
        }
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось загрузить список банков:\n" + query.lastError().text());
    }
}

void ApplicantReg::loadPassportDepartments()
{
    ui->passportIssuingDepartment->clear();
    ui->passportIssuingDepartment->addItem("Выберите отдел", -1);

    QSqlQuery query;
    query.prepare("SELECT passport_issuing_department_id, issuing_department FROM passport_issuing_department ORDER BY issuing_department");

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

void ApplicantReg::on_noPatronymicCheck_toggled(bool checked)
{
    ui->patronymic->setEnabled(!checked);
    if (checked) {
        ui->patronymic->clear();
    }
}

void ApplicantReg::on_noActualAddress_toggled(bool checked)
{
    enableActualAddressFields(!checked);
}

void ApplicantReg::on_noAdoptationCertificate_toggled(bool checked)
{
    enableAdoptionCertificateFields(!checked);
}

void ApplicantReg::enableActualAddressFields(bool enabled)
{
    ui->regionComboBoxActualAddress->setEnabled(enabled);
    ui->districtComboBoxActualAddress->setEnabled(enabled);
    ui->cityComboBoxActualAddress->setEnabled(enabled);
    ui->areaComboBoxActualAddress->setEnabled(enabled);
    ui->StreetComboBoxActualAddress->setEnabled(enabled);
    ui->houseComboBoxActualAddress->setEnabled(enabled);
    ui->flatComboBoxActualAddress->setEnabled(enabled);
    ui->indexEdit_2->setEnabled(enabled);
    ui->districtCheckActualAddress->setEnabled(enabled);
    ui->areaCheckActualAddress->setEnabled(enabled);
    ui->flatCheckActualAddress->setEnabled(enabled);

    if (!enabled) {
        ui->regionComboBoxActualAddress->setCurrentIndex(0);
        ui->districtComboBoxActualAddress->clearEditText();
        ui->cityComboBoxActualAddress->clearEditText();
        ui->areaComboBoxActualAddress->clearEditText();
        ui->StreetComboBoxActualAddress->clearEditText();
        ui->houseComboBoxActualAddress->clearEditText();
        ui->flatComboBoxActualAddress->clearEditText();
        ui->indexEdit_2->clear();
    }
}

void ApplicantReg::enableAdoptionCertificateFields(bool enabled)
{
    ui->adopatationCertificateSeriesEdit->setEnabled(enabled);
    ui->adopatationCertificateNumberEdit->setEnabled(enabled);
    ui->adopatationCertificateAddScan->setEnabled(enabled);

    if (!enabled) {
        ui->adopatationCertificateSeriesEdit->clear();
        ui->adopatationCertificateNumberEdit->clear();
        adoptionCertificateScanData = QByteArray();
    }
}

void ApplicantReg::on_passportScan_clicked()
{
    QByteArray scanData = loadScanFile("Выберите скан паспорта (PNG)");
    if (!scanData.isEmpty()) {
        passportScanData = scanData;
        QMessageBox::information(this, "Успех", "Скан паспорта загружен");
    }
}

void ApplicantReg::on_snilsAddScan_clicked()
{
    QByteArray scanData = loadScanFile("Выберите скан СНИЛС (PNG)");
    if (!scanData.isEmpty()) {
        snilsScanData = scanData;
        QMessageBox::information(this, "Успех", "Скан СНИЛС загружен");
    }
}

void ApplicantReg::on_policyAddScan_clicked()
{
    QByteArray scanData = loadScanFile("Выберите скан полиса ОМС (PNG)");
    if (!scanData.isEmpty()) {
        policyScanData = scanData;
        QMessageBox::information(this, "Успех", "Скан полиса ОМС загружен");
    }
}

void ApplicantReg::on_adopatationCertificateAddScan_clicked()
{
    QByteArray scanData = loadScanFile("Выберите скан свидетельства об усыновлении (PNG)");
    if (!scanData.isEmpty()) {
        adoptionCertificateScanData = scanData;
        QMessageBox::information(this, "Успех", "Скан свидетельства об усыновлении загружен");
    }
}

QByteArray ApplicantReg::loadScanFile(const QString &title)
{
    QString fileName = QFileDialog::getOpenFileName(this, title,
                                                    QDir::homePath(),
                                                    "PNG Images (*.png)");

    if (fileName.isEmpty()) {
        return QByteArray();
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл: " + file.errorString());
        return QByteArray();
    }

    QByteArray data = file.readAll();
    file.close();

    // Проверяем, что файл действительно PNG
    if (data.size() > 8 && data.startsWith("\x89PNG\r\n\x1a\n")) {
        return data;
    } else {
        QMessageBox::warning(this, "Ошибка", "Файл должен быть в формате PNG");
        return QByteArray();
    }
}

void ApplicantReg::on_applicantRegOK_clicked()
{
    // === ВАЛИДАЦИЯ ПОЛЕЙ ===

    // 1. Данные для входа
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

    // 2. Основные данные
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

    // 3. Паспортные данные
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

    // 4. Адрес регистрации
    if (ui->regionComboBoxRegAddress->currentData().toInt() == -1) {
        QMessageBox::warning(this, "Ошибка", "Выберите регион регистрации");
        return;
    }

    if (ui->cityComboBoxRegAddress->currentText().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите город регистрации");
        return;
    }

    if (ui->StreetComboBoxRegAddress->currentText().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите улицу регистрации");
        return;
    }

    if (ui->houseComboBoxRegAddress->currentText().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите дом регистрации");
        return;
    }

    // 5. Банковские данные
    if (ui->bankName->currentText().isEmpty() || ui->bankName->currentText() == "Выберите банк или введите вручную") {
        QMessageBox::warning(this, "Ошибка", "Введите название банка");
        return;
    }

    if (ui->surnameBank->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите фамилию получателя");
        return;
    }

    if (ui->nameBank->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите имя получателя");
        return;
    }

    if (ui->bankPhone->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите номер телефона банка получателя");
        return;
    }

    // === ПОДГОТОВКА ДАННЫХ ===

    // Данные для аккаунта
    QString username = ui->login->text();
    QString password = ui->password->text();
    QString email = ui->eMail->text();

    // ФИО и основные данные
    QString surname = ui->surname->text();
    QString name = ui->name->text();
    QString patronymic = ui->noPatronymicCheck->isChecked() ? "" : ui->patronymic->text();
    QString inn = ui->lineEdit_6->text();
    double pension_coefficient = ui->doubleSpinBox->value();
    QString phone_number = ui->phone->text();

    // Паспортные данные
    QString passport_series = ui->passportSeriesEdit->text();
    QString passport_number = ui->passportNumberEdit->text();
    QString passport_issued_by = ui->passportIssuingDepartment->currentText();
    QDate passport_issue_date = ui->passportDateOfIssue->date();
    QString passport_division_code = ui->passportDivisionCode->text();
    QByteArray passport_scan = passportScanData;
    QDate birth_date = ui->birthDate->date();

    // Данные СНИЛС
    QString snils_number = ui->snilsNumberEdit->text();
    QByteArray snils_scan = snilsScanData;

    // Данные полиса
    QString policy_number = ui->policyNumberEdit->text();
    QByteArray policy_scan = policyScanData;

    // Данные свидетельства об усыновлении
    QString adoption_certificate_series = ui->noAdoptationCertificate->isChecked() ? "" : ui->adopatationCertificateSeriesEdit->text();
    QString adoption_certificate_number = ui->noAdoptationCertificate->isChecked() ? "" : ui->adopatationCertificateNumberEdit->text();
    QByteArray adoption_certificate_scan = adoptionCertificateScanData;

    // Адрес регистрации
    QString reg_region = ui->regionComboBoxRegAddress->currentText();
    QString reg_district = ui->districtCheckRegAddress->isChecked() ? "" : ui->districtComboBoxRegAddress->currentText();
    QString reg_city = ui->cityComboBoxRegAddress->currentText();
    QString reg_area = ui->areaCheckRegAddress->isChecked() ? "" : ui->areaComboBoxRegAddress->currentText();
    QString reg_street = ui->StreetComboBoxRegAddress->currentText();
    QString reg_house = ui->houseComboBoxRegAddress->currentText();
    QString reg_flat = ui->flatCheckRegAddress->isChecked() ? "" : ui->flatComboBoxRegAddress->currentText();
    QString reg_postal_code = ui->indexEditRegAddress->text();

    // Адрес фактического проживания
    QString act_region = ui->noActualAddress->isChecked() ? reg_region : ui->regionComboBoxActualAddress->currentText();
    QString act_district = ui->noActualAddress->isChecked() ? reg_district :
                               (ui->districtCheckActualAddress->isChecked() ? "" : ui->districtComboBoxActualAddress->currentText());
    QString act_city = ui->noActualAddress->isChecked() ? reg_city : ui->cityComboBoxActualAddress->currentText();
    QString act_area = ui->noActualAddress->isChecked() ? reg_area :
                           (ui->areaCheckActualAddress->isChecked() ? "" : ui->areaComboBoxActualAddress->currentText());
    QString act_street = ui->noActualAddress->isChecked() ? reg_street : ui->StreetComboBoxActualAddress->currentText();
    QString act_house = ui->noActualAddress->isChecked() ? reg_house : ui->houseComboBoxActualAddress->currentText();
    QString act_flat = ui->noActualAddress->isChecked() ? reg_flat :
                           (ui->flatCheckActualAddress->isChecked() ? "" : ui->flatComboBoxActualAddress->currentText());
    QString act_postal_code = ui->noActualAddress->isChecked() ? reg_postal_code : ui->indexEdit_2->text();

    // Данные банковского счета
    QString recipient_surname = ui->surnameBank->text();
    QString recipient_name = ui->nameBank->text();
    QString recipient_patronymic = ui->noPatronymicCheckBank->isChecked() ? "" : ui->patronymicBank->text();
    QString recipient_account_number = ui->recepientsAccount->text();
    QDate account_opening_date = ui->openningDate->date();

    // Данные банка
    QString bank_name = ui->bankName->currentText(); // Берем текст из комбобокса (может быть введен вручную)
    QString bank_bic = ui->bikEdit->text();
    QString bank_inn = ui->innEdit->text();
    QString bank_kpp = ui->kppEdit->text();
    QString bank_correspondent_account = ui->correspondentsAccountEdit->text();
    QString bank_phone = ui->bankPhone->text();

    // === ВЫЗОВ ХРАНИМОЙ ПРОЦЕДУРЫ ===

    QSqlQuery query;
    query.prepare("CALL new_applicant("
                  ":p_username, :p_password, :p_email, "
                  ":p_surname, :p_name, :p_patronymic, :p_inn, :p_pension_coefficient, :p_phone_number, "
                  ":p_passport_series, :p_passport_number, :p_passport_issued_by, :p_passport_issue_date, "
                  ":p_passport_division_code, :p_passport_scan, :p_birth_date, "
                  ":p_snils_number, :p_snils_scan, "
                  ":p_policy_number, :p_policy_scan, "
                  ":p_adoption_certificate_series, :p_adoption_certificate_number, :p_adoption_certificate_scan, "
                  ":p_reg_region, :p_reg_district, :p_reg_city, :p_reg_area, :p_reg_street, :p_reg_house, :p_reg_flat, :p_reg_postal_code, "
                  ":p_act_region, :p_act_district, :p_act_city, :p_act_area, :p_act_street, :p_act_house, :p_act_flat, :p_act_postal_code, "
                  ":p_recipient_surname, :p_recipient_name, :p_recipient_patronymic, :p_recipient_account_number, :p_account_opening_date, "
                  ":p_bank_name, :p_bank_bic, :p_bank_inn, :p_bank_kpp, :p_bank_correspondent_account, :p_bank_phone)");

    // Данные для аккаунта
    query.bindValue(":p_username", username);
    query.bindValue(":p_password", password);
    query.bindValue(":p_email", email);

    // ФИО и основные данные
    query.bindValue(":p_surname", surname);
    query.bindValue(":p_name", name);
    query.bindValue(":p_patronymic", patronymic);
    query.bindValue(":p_inn", inn);
    query.bindValue(":p_pension_coefficient", pension_coefficient);
    query.bindValue(":p_phone_number", phone_number);

    // Паспортные данные
    query.bindValue(":p_passport_series", passport_series);
    query.bindValue(":p_passport_number", passport_number);
    query.bindValue(":p_passport_issued_by", passport_issued_by);
    query.bindValue(":p_passport_issue_date", passport_issue_date);
    query.bindValue(":p_passport_division_code", passport_division_code);
    query.bindValue(":p_passport_scan", passport_scan);
    query.bindValue(":p_birth_date", birth_date);

    // Данные СНИЛС
    query.bindValue(":p_snils_number", snils_number);
    query.bindValue(":p_snils_scan", snils_scan);

    // Данные полиса
    query.bindValue(":p_policy_number", policy_number);
    query.bindValue(":p_policy_scan", policy_scan);

    // Данные свидетельства об усыновлении
    query.bindValue(":p_adoption_certificate_series", adoption_certificate_series);
    query.bindValue(":p_adoption_certificate_number", adoption_certificate_number);
    query.bindValue(":p_adoption_certificate_scan", adoption_certificate_scan);

    // Адрес регистрации
    query.bindValue(":p_reg_region", reg_region);
    query.bindValue(":p_reg_district", reg_district);
    query.bindValue(":p_reg_city", reg_city);
    query.bindValue(":p_reg_area", reg_area);
    query.bindValue(":p_reg_street", reg_street);
    query.bindValue(":p_reg_house", reg_house);
    query.bindValue(":p_reg_flat", reg_flat);
    query.bindValue(":p_reg_postal_code", reg_postal_code);

    // Адрес фактического проживания
    query.bindValue(":p_act_region", act_region);
    query.bindValue(":p_act_district", act_district);
    query.bindValue(":p_act_city", act_city);
    query.bindValue(":p_act_area", act_area);
    query.bindValue(":p_act_street", act_street);
    query.bindValue(":p_act_house", act_house);
    query.bindValue(":p_act_flat", act_flat);
    query.bindValue(":p_act_postal_code", act_postal_code);

    // Данные банковского счета
    query.bindValue(":p_recipient_surname", recipient_surname);
    query.bindValue(":p_recipient_name", recipient_name);
    query.bindValue(":p_recipient_patronymic", recipient_patronymic);
    query.bindValue(":p_recipient_account_number", recipient_account_number);
    query.bindValue(":p_account_opening_date", account_opening_date);

    // Данные банка
    query.bindValue(":p_bank_name", bank_name);
    query.bindValue(":p_bank_bic", bank_bic);
    query.bindValue(":p_bank_inn", bank_inn);
    query.bindValue(":p_bank_kpp", bank_kpp);
    query.bindValue(":p_bank_correspondent_account", bank_correspondent_account);
    query.bindValue(":p_bank_phone", bank_phone);

    if (query.exec()) {
        QMessageBox::information(this, "Успех", "Заявитель успешно зарегистрирован!");
        MainWindow* mw = new MainWindow();
        this->close();
        mw->show();
    } else {
        QString errorText = query.lastError().text();
        QMessageBox::critical(this, "Ошибка регистрации",
                              "Не удалось зарегистрировать заявителя:\n" + errorText);
    }
}
