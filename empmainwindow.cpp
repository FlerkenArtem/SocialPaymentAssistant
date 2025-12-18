#include "empmainwindow.h"
#include "about.h"
#include "loginwidget.h"
#include "ui_empmainwindow.h"
#include "pdfdocumentgenerator.h"
#include <QSqlQuery>
#include <QSqlRecord>
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
        "SELECT e.surname, e.name, e.patronymic, ep.position "
        "FROM employee e "
        "JOIN employee_position ep ON e.employee_position_id = ep.employee_position_id "
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
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось загрузить данные сотрудника: " + query.lastError().text());
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

    try {
        // Получаем текущую сумму заявки
        QSqlQuery amountQuery;
        amountQuery.prepare("SELECT amount FROM application WHERE application_id = :id");
        amountQuery.bindValue(":id", m_currentApplicationId);
        double requestedAmount = 0;
        if (amountQuery.exec() && amountQuery.next()) {
            requestedAmount = amountQuery.value(0).toDouble();
            qDebug() << "Запрашиваемая сумма:" << requestedAmount;
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

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Подтверждение принятия",
                                      QString("Вы уверены, что хотите принять заявку?\n"
                                              "Сумма выплаты: %1 руб.\n"
                                              "Данное действие необратимо.")
                                          .arg(approvedAmount, 0, 'f', 2),
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            // Вызов хранимой процедуры accept_application
            QSqlQuery query;
            query.prepare("CALL accept_application(:application_id, :employee_password, :new_amount)");
            query.bindValue(":application_id", m_currentApplicationId);
            query.bindValue(":employee_password", password);
            query.bindValue(":new_amount", approvedAmount);

            if (query.exec()) {
                qDebug() << "Хранимая процедура accept_application выполнена успешно";

                // Генерация и сохранение PDF-документа
                generateAndSaveCertificate(m_currentApplicationId, requestedAmount, approvedAmount);

                QMessageBox::information(this, "Успех", "Заявка успешно принята и документ сформирован");
                refreshApplicationData();
                ui->textBrowser->clear();
                m_currentApplicationId = -1;
                ui->tabWidget->setCurrentIndex(0);
            } else {
                QString error = query.lastError().text();
                QMessageBox::critical(this, "Ошибка",
                                      "Не удалось принять заявку:\n" + error);
                qDebug() << "Ошибка при выполнении хранимой процедуры:" << error;
            }
        }
    } catch (const std::exception &e) {
        QMessageBox::critical(this, "Критическая ошибка",
                              QString("Произошла критическая ошибка:\n%1").arg(e.what()));
        qCritical() << "Критическая ошибка в on_accept_clicked:" << e.what();
    } catch (...) {
        QMessageBox::critical(this, "Неизвестная ошибка",
                              "Произошла неизвестная критическая ошибка");
        qCritical() << "Неизвестная критическая ошибка в on_accept_clicked";
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
        "tosp.type_name, "
        "a.amount, "
        "a.date_of_creation, "
        "ast.status, "
        "e.surname || ' ' || e.name || COALESCE(' ' || e.patronymic, '') as employee_fio, "
        "ep.position, "
        "app.inn, "
        "app.phone_number, "
        "app.applicant_id "
        "FROM application a "
        "JOIN applicant app ON a.applicant_id = app.applicant_id "
        "JOIN type_of_social_payment tosp ON a.type_of_social_payment_id = tosp.type_of_social_payment_id "
        "JOIN application_status ast ON a.application_status_id = ast.application_status_id "
        "JOIN employee e ON a.employee_id = e.employee_id "
        "JOIN employee_position ep ON e.employee_position_id = ep.employee_position_id "
        "WHERE a.application_id = :application_id"
        );
    query.bindValue(":application_id", applicationId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка запроса",
                              "Не удалось выполнить запрос:\n" + query.lastError().text());
        return;
    }

    if (query.next()) {
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
                              "<tr><td><b>ID заявителя:</b></td><td>%11</td></tr>"
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
                              .arg(query.value(7).toString())
                              .arg(query.value(10).toInt());

        ui->textBrowser->setHtml(details);
    } else {
        QMessageBox::warning(this, "Предупреждение",
                             "Заявка с ID " + QString::number(applicationId) + " не найдена в базе данных.");
    }
}

void EmpMainWindow::generateAndSaveCertificate(int applicationId, double requestedAmount, double approvedAmount)
{
    qDebug() << "Начинаем генерацию сертификата для заявки:" << applicationId;

    // Получаем базовые данные для справки (упрощенный запрос)
    QSqlQuery query;
    query.prepare(
        "SELECT "
        "a.application_id, "
        "app.surname || ' ' || app.name || COALESCE(' ' || app.patronymic, '') as applicant_fio, "
        "tosp.type_name, "
        "a.date_of_creation, "
        "e.surname || ' ' || e.name || COALESCE(' ' || e.patronymic, '') as employee_fio, "
        "ep.position, "
        "app.inn, "
        "app.phone_number "
        "FROM application a "
        "JOIN applicant app ON a.applicant_id = app.applicant_id "
        "JOIN type_of_social_payment tosp ON a.type_of_social_payment_id = tosp.type_of_social_payment_id "
        "JOIN employee e ON a.employee_id = e.employee_id "
        "JOIN employee_position ep ON e.employee_position_id = ep.employee_position_id "
        "WHERE a.application_id = :application_id"
        );
    query.bindValue(":application_id", applicationId);

    if (!query.exec()) {
        QMessageBox::critical(this, "Ошибка SQL",
                              "Ошибка выполнения запроса для справки:\n" + query.lastError().text());
        return;
    }

    if (!query.next()) {
        QMessageBox::warning(this, "Предупреждение",
                             "Не удалось получить данные для справки (заявка ID: " + QString::number(applicationId) + ")");
        return;
    }

    qDebug() << "Данные для справки получены";

    // Заполняем структуру ApplicationInfo
    PdfDocumentGenerator::ApplicationInfo appInfo;
    appInfo.applicationId = applicationId;
    appInfo.applicantName = query.value(1).toString();
    appInfo.paymentType = query.value(2).toString();
    appInfo.creationDate = query.value(3).toDateTime();
    appInfo.requestedAmount = requestedAmount;
    appInfo.approvedAmount = approvedAmount;
    appInfo.employeeName = query.value(4).toString();
    appInfo.employeePosition = query.value(5).toString();
    appInfo.branchName = "Отделение социальных выплат"; // Упрощенно
    appInfo.applicantInn = query.value(6).toString();
    appInfo.applicantPhone = query.value(7).toString();
    appInfo.applicantAddress = "Адрес по регистрации"; // Упрощенно

    // Генерируем PDF-документ
    PdfDocumentGenerator generator;
    QByteArray pdfData = generator.generateCertificate(appInfo);

    if (pdfData.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сгенерировать PDF-документ");
        return;
    }

    qDebug() << "PDF сгенерирован, размер:" << pdfData.size() << "байт";

    // Проверяем, есть ли уже сертификат для этой заявки
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT COUNT(*) FROM certificate WHERE application_id = :application_id");
    checkQuery.bindValue(":application_id", applicationId);

    if (checkQuery.exec() && checkQuery.next()) {
        int count = checkQuery.value(0).toInt();
        if (count > 0) {
            // Обновляем существующий сертификат
            QSqlQuery updateQuery;
            updateQuery.prepare(
                "UPDATE certificate SET date_and_time_of_creation = :creation_date, document = :document "
                "WHERE application_id = :application_id"
                );
            updateQuery.bindValue(":application_id", applicationId);
            updateQuery.bindValue(":creation_date", QDateTime::currentDateTime());
            updateQuery.bindValue(":document", pdfData);

            if (!updateQuery.exec()) {
                QMessageBox::warning(this, "Ошибка обновления",
                                     "Не удалось обновить документ в БД:\n" + updateQuery.lastError().text());
                qDebug() << "Ошибка SQL при обновлении:" << updateQuery.lastError().text();
            } else {
                qDebug() << "Сертификат обновлен для application_id:" << applicationId;
            }
        } else {
            // Вставляем новый сертификат
            QSqlQuery insertQuery;
            insertQuery.prepare(
                "INSERT INTO certificate (application_id, date_and_time_of_creation, document) "
                "VALUES (:application_id, :creation_date, :document)"
                );
            insertQuery.bindValue(":application_id", applicationId);
            insertQuery.bindValue(":creation_date", QDateTime::currentDateTime());
            insertQuery.bindValue(":document", pdfData);

            if (!insertQuery.exec()) {
                QMessageBox::warning(this, "Ошибка сохранения",
                                     "Не удалось сохранить документ в БД:\n" + insertQuery.lastError().text());
                qDebug() << "Ошибка SQL при вставке:" << insertQuery.lastError().text();
            } else {
                qDebug() << "Сертификат создан для application_id:" << applicationId;
            }
        }
    }

    // Проверяем результат
    QSqlQuery verifyQuery;
    verifyQuery.prepare("SELECT certificate_id, date_and_time_of_creation, LENGTH(document) as doc_size "
                        "FROM certificate WHERE application_id = :application_id");
    verifyQuery.bindValue(":application_id", applicationId);

    if (verifyQuery.exec() && verifyQuery.next()) {
        qDebug() << "Сертификат успешно сохранен в БД:";
        qDebug() << "ID сертификата:" << verifyQuery.value(0).toInt();
        qDebug() << "Дата создания:" << verifyQuery.value(1).toDateTime();
        qDebug() << "Размер документа:" << verifyQuery.value(2).toInt() << "байт";
    }
}
