#include "empmainwindow.h"
#include "about.h"
#include "loginwidget.h"
#include "ui_empmainwindow.h"
#include "pdfdocumentgenerator.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>
#include <QFileDialog>
#include <QTextDocument>
#include <QPrinter>

EmpMainWindow::EmpMainWindow(int employeeId, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EmpMainWindow)
    , m_employeeId(employeeId)
    , m_applicationsModel(nullptr)
    , m_currentApplicationId(-1)
{
    ui->setupUi(this);

    // Получаем информацию о сотруднике для отображения
    QSqlQuery query;
    query.prepare(
        "SELECT surname, name, patronymic, position_name "
        "FROM employee e "
        "JOIN position p ON e.position_id = p.position_id "
        "WHERE e.employee_id = :employee_id"
        );
    query.bindValue(":employee_id", m_employeeId);

    if (query.exec() && query.next()) {
        QString surname = query.value(0).toString();
        QString name = query.value(1).toString();
        QString patronymic = query.value(2).toString();
        QString position = query.value(3).toString();

        QString title = QString("Сотрудник: %1 %2 %3 (%4)")
                            .arg(surname).arg(name).arg(patronymic).arg(position);
        setWindowTitle(title);
    }

    // Инициализируем модель заявок
    m_applicationsModel = new EmpApplicationsModel(m_employeeId, this);
    ui->tableView->setModel(m_applicationsModel);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setColumnHidden(0, true); // Скрываем ID

    // Устанавливаем начальную вкладку
    ui->tabWidget->setCurrentIndex(0);
}

EmpMainWindow::~EmpMainWindow()
{
    delete ui;
}

void EmpMainWindow::on_open_clicked()
{
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите заявку для просмотра");
        return;
    }

    int sourceRow = selected.first().row();
    EmpApplicationData app = m_applicationsModel->getApplicationData(sourceRow);
    m_currentApplicationId = app.applicationId;

    showApplicationDetails(app.applicationId);
    ui->tabWidget->setCurrentIndex(1); // Переключаемся на вкладку чтения
}

void EmpMainWindow::on_cancel_clicked()
{
    if (m_currentApplicationId == -1) {
        QMessageBox::warning(this, "Ошибка", "Сначала откройте заявку");
        return;
    }

    // Запрос подтверждения
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Подтверждение отклонения");
    msgBox.setText("Вы уверены, что хотите отклонить заявку?");
    msgBox.setInformativeText("Данное действие необратимо. Для подтверждения введите пароль.");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    bool ok;
    QString password = QInputDialog::getText(this, "Подтверждение пароля",
                                             "Введите ваш пароль:",
                                             QLineEdit::Password, "", &ok);

    if (!ok || password.isEmpty()) {
        return;
    }

    if (msgBox.exec() == QMessageBox::Yes) {
        // Вызов хранимой процедуры cancel_application
        QSqlQuery query;
        query.prepare("CALL cancel_application(:application_id, :employee_password)");
        query.bindValue(":application_id", m_currentApplicationId);
        query.bindValue(":employee_password", password);

        if (query.exec()) {
            QMessageBox::information(this, "Успех", "Заявка успешно отклонена");
            refreshApplicationData();
            ui->textBrowser->clear();
            m_currentApplicationId = -1;
            ui->tabWidget->setCurrentIndex(0); // Возвращаемся к списку
        } else {
            QMessageBox::critical(this, "Ошибка",
                                  "Не удалось отклонить заявку:\n" + query.lastError().text());
        }
    }
}

void EmpMainWindow::on_accept_clicked()
{
    if (m_currentApplicationId == -1) {
        QMessageBox::warning(this, "Ошибка", "Сначала откройте заявку");
        return;
    }

    // Получаем текущую сумму заявки
    QSqlQuery amountQuery;
    amountQuery.prepare("SELECT amount FROM application WHERE application_id = :id");
    amountQuery.bindValue(":id", m_currentApplicationId);
    double requestedAmount = 0;
    if (amountQuery.exec() && amountQuery.next()) {
        requestedAmount = amountQuery.value(0).toDouble();
    }

    // Диалог для подтверждения и изменения суммы
    bool ok;
    QString password = QInputDialog::getText(this, "Подтверждение принятия",
                                             "Введите ваш пароль:",
                                             QLineEdit::Password, "", &ok);

    if (!ok || password.isEmpty()) {
        return;
    }

    double approvedAmount = QInputDialog::getDouble(this, "Сумма выплаты",
                                                    "Введите сумму выплаты:",
                                                    requestedAmount, 0, 10000000, 2, &ok);
    if (!ok) {
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Подтверждение принятия");
    msgBox.setText("Вы уверены, что хотите принять заявку?");
    msgBox.setInformativeText(QString("Сумма выплаты: %1 руб.\nДанное действие необратимо.")
                                  .arg(approvedAmount, 0, 'f', 2));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);

    if (msgBox.exec() == QMessageBox::Yes) {
        // Вызов хранимой процедуры accept_application
        QSqlQuery query;
        query.prepare("CALL accept_application(:application_id, :employee_password, :new_amount)");
        query.bindValue(":application_id", m_currentApplicationId);
        query.bindValue(":employee_password", password);
        query.bindValue(":new_amount", approvedAmount);

        if (query.exec()) {
            // Генерация и сохранение PDF-документа с использованием PdfDocumentGenerator
            generateAndSaveCertificate(m_currentApplicationId, requestedAmount, approvedAmount);

            QMessageBox::information(this, "Успех", "Заявка успешно принята и документ сформирован");
            refreshApplicationData();
            ui->textBrowser->clear();
            m_currentApplicationId = -1;
            ui->tabWidget->setCurrentIndex(0); // Возвращаемся к списку
        } else {
            QMessageBox::critical(this, "Ошибка",
                                  "Не удалось принять заявку:\n" + query.lastError().text());
        }
    }
}

void EmpMainWindow::on_empLogoutAct_triggered()
{
    LoginWidget *login = new LoginWidget();
    login->show();
    this->close();
}

void EmpMainWindow::on_empFullscreenAct_triggered()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

void EmpMainWindow::on_empQuitAct_triggered()
{
    qApp->quit();
}

void EmpMainWindow::on_empAboutAct_triggered()
{
    About *aboutWidget = new About();
    aboutWidget->setAttribute(Qt::WA_DeleteOnClose);
    int tabIndex = ui->tabWidget->addTab(aboutWidget, "О программе");
    ui->tabWidget->setCurrentIndex(tabIndex);

    connect(aboutWidget, &About::destroyed, [this, tabIndex]() {
        ui->tabWidget->removeTab(tabIndex);
    });
}

void EmpMainWindow::refreshApplicationData()
{
    m_applicationsModel->refreshData();
}

void EmpMainWindow::showApplicationDetails(int applicationId)
{
    QSqlQuery query;
    query.prepare(
        "SELECT "
        "a.application_id, "
        "app.surname || ' ' || app.name || COALESCE(' ' || app.patronymic, '') as applicant_fio, "
        "pt.type_name, "
        "a.amount, "
        "a.date_of_creation, "
        "ast.status, "
        "e.surname || ' ' || e.name || COALESCE(' ' || e.patronymic, '') as employee_fio, "
        "p.position_name, "
        "app.inn, "
        "app.phone_number "
        "FROM application a "
        "JOIN applicant app ON a.applicant_id = app.applicant_id "
        "JOIN type_of_social_payment pt ON a.type_of_social_payment_id = pt.type_of_social_payment_id "
        "JOIN application_status ast ON a.application_status_id = ast.application_status_id "
        "JOIN employee e ON a.employee_id = e.employee_id "
        "JOIN position p ON e.position_id = p.position_id "
        "WHERE a.application_id = :application_id"
        );
    query.bindValue(":application_id", applicationId);

    if (query.exec() && query.next()) {
        QString details = QString(
                              "<h2>Детали заявки #%1</h2>"
                              "<table border='1' cellpadding='5' style='border-collapse: collapse;'>"
                              "<tr><td><b>ID заявки:</b></td><td>%1</td></tr>"
                              "<tr><td><b>ФИО заявителя:</b></td><td>%2</td></tr>"
                              "<tr><td><b>ИНН заявителя:</b></td><td>%3</td></tr>"
                              "<tr><td><b>Телефон заявителя:</b></td><td>%4</td></tr>"
                              "<tr><td><b>Тип выплаты:</b></td><td>%5</td></tr>"
                              "<tr><td><b>Сумма:</b></td><td>%6 руб.</td></tr>"
                              "<tr><td><b>Дата подачи:</b></td><td>%7</td></tr>"
                              "<tr><td><b>Статус:</b></td><td>%8</td></tr>"
                              "<tr><td><b>Ответственный сотрудник:</b></td><td>%9</td></tr>"
                              "<tr><td><b>Должность:</b></td><td>%10</td></tr>"
                              "</table>"
                              "<p><i>Для принятия или отклонения заявки используйте кнопки ниже</i></p>"
                              )
                              .arg(query.value(0).toInt())
                              .arg(query.value(1).toString())
                              .arg(query.value(8).toString())
                              .arg(query.value(9).toString())
                              .arg(query.value(2).toString())
                              .arg(QString::number(query.value(3).toDouble(), 'f', 2))
                              .arg(query.value(4).toDateTime().toString("dd.MM.yyyy HH:mm"))
                              .arg(query.value(5).toString())
                              .arg(query.value(6).toString())
                              .arg(query.value(7).toString());

        ui->textBrowser->setHtml(details);
    }
}

void EmpMainWindow::generateAndSaveCertificate(int applicationId, double requestedAmount, double approvedAmount)
{
    // Получаем полные данные для справки
    QSqlQuery query;
    query.prepare(
        "SELECT "
        "a.application_id, "
        "app.surname || ' ' || app.name || COALESCE(' ' || app.patronymic, '') as applicant_fio, "
        "pt.type_name, "
        "a.date_of_creation, "
        "e.surname || ' ' || e.name || COALESCE(' ' || e.patronymic, '') as employee_fio, "
        "p.position_name, "
        "b.name_branch, "
        "app.inn, "
        "app.phone_number, "
        "addr_reg.postal_code, "
        "reg.name_region, "
        "city.name_city, "
        "street.name_street, "
        "house.name_house, "
        "COALESCE(flat.name_flat, '-') "
        "FROM application a "
        "JOIN applicant app ON a.applicant_id = app.applicant_id "
        "JOIN type_of_social_payment pt ON a.type_of_social_payment_id = pt.type_of_social_payment_id "
        "JOIN employee e ON a.employee_id = e.employee_id "
        "JOIN position p ON e.position_id = p.position_id "
        "JOIN branch b ON e.branch_id = b.branch_id "
        "JOIN address addr_reg ON app.registration_address_id = addr_reg.address_id "
        "JOIN region reg ON addr_reg.region_id = reg.region_id "
        "JOIN city ON addr_reg.city_id = city.city_id "
        "JOIN street ON addr_reg.street_id = street.street_id "
        "JOIN house ON addr_reg.house_id = house.house_id "
        "LEFT JOIN flat ON addr_reg.flat_id = flat.flat_id "
        "WHERE a.application_id = :application_id"
        );
    query.bindValue(":application_id", applicationId);

    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось получить данные для справки");
        return;
    }

    // Заполняем структуру ApplicationInfo для PdfDocumentGenerator
    PdfDocumentGenerator::ApplicationInfo appInfo;
    appInfo.applicationId = applicationId;
    appInfo.applicantName = query.value(1).toString();
    appInfo.paymentType = query.value(2).toString();
    appInfo.creationDate = query.value(3).toDateTime();
    appInfo.requestedAmount = requestedAmount;
    appInfo.approvedAmount = approvedAmount;
    appInfo.employeeName = query.value(4).toString();
    appInfo.employeePosition = query.value(5).toString();
    appInfo.branchName = query.value(6).toString();
    appInfo.applicantInn = query.value(7).toString();
    appInfo.applicantPhone = query.value(8).toString();

    // Формируем адрес
    appInfo.applicantAddress = QString("%1, %2, %3, %4, кв. %5")
                                   .arg(query.value(9).toString())  // postal_code
                                   .arg(query.value(10).toString()) // region
                                   .arg(query.value(11).toString()) // city
                                   .arg(query.value(12).toString()) // street, house
                                   .arg(query.value(13).toString()); // flat

    // Создаем генератор PDF
    PdfDocumentGenerator generator;

    // Опционально: сохранить в файл (для отладки)
    QString fileName = QString("Справка_%1_%2.pdf")
                           .arg(applicationId)
                           .arg(QDateTime::currentDateTime().toString("ddMMyyyy_HHmmss"));

    // Сохраняем в файл для проверки
    if (generator.saveToFile(fileName, appInfo)) {
        // Читаем файл для сохранения в БД
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray pdfData = file.readAll();
            file.close();

            // Сохраняем PDF в БД как BYTEA
            QSqlQuery insertQuery;
            insertQuery.prepare(
                "INSERT INTO certificate (application_id, date_and_time_of_creation, document) "
                "VALUES (:application_id, :creation_date, :document)"
                );
            insertQuery.bindValue(":application_id", applicationId);
            insertQuery.bindValue(":creation_date", QDateTime::currentDateTime());
            insertQuery.bindValue(":document", pdfData);

            if (!insertQuery.exec()) {
                QMessageBox::warning(this, "Ошибка",
                                     "Не удалось сохранить документ в БД:\n" + insertQuery.lastError().text());
            }

            // Удаляем временный файл
            QFile::remove(fileName);
        }
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось сгенерировать PDF-документ");
    }
}
