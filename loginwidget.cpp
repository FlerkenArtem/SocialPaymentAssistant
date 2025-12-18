#include "loginwidget.h"
#include "ui_loginwidget.h"
#include "registrationwidget.h"
#include "mainwindow.h"
#include "empmainwindow.h"
#include <QSqlError>

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::on_endLoginButton_clicked()
{
    QString username = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Логин и пароль не могут быть пустыми");
        return;
    }

    // Вызов хранимой функции authenticate_user из БД
    QSqlQuery query;
    query.prepare("SELECT authenticate_user(:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);

    if (query.exec() && query.next()) {
        int roleId = query.value(0).toInt();

        // В зависимости от роли открываем соответствующее окно
        if (roleId == 1) { // Заявитель
            // Получаем ID заявителя
            QSqlQuery applicantQuery;
            applicantQuery.prepare(
                "SELECT a.applicant_id "
                "FROM applicant a "
                "JOIN account ac ON a.account_id = ac.account_id "
                "WHERE ac.username = :login"
                );
            applicantQuery.bindValue(":login", username);

            if (applicantQuery.exec() && applicantQuery.next()) {
                int applicantId = applicantQuery.value(0).toInt();
                MainWindow *mainWindow = new MainWindow(applicantId);
                mainWindow->show();
                this->close(); // Закрываем окно входа
            } else {
                QMessageBox::critical(this, "Ошибка",
                                      "Не удалось получить данные заявителя:\n" + applicantQuery.lastError().text());
            }
        }
        else if (roleId == 2) { // Сотрудник
            // Получаем ID сотрудника
            QSqlQuery employeeQuery;
            employeeQuery.prepare(
                "SELECT e.employee_id "
                "FROM employee e "
                "JOIN account ac ON e.account_id = ac.account_id "
                "WHERE ac.username = :login"
                );
            employeeQuery.bindValue(":login", username);

            if (employeeQuery.exec() && employeeQuery.next()) {
                int employeeId = employeeQuery.value(0).toInt();
                EmpMainWindow *empWindow = new EmpMainWindow(employeeId);
                empWindow->show();
                this->close(); // Закрываем окно входа
            } else {
                QMessageBox::critical(this, "Ошибка",
                                      "Не удалось получить данные сотрудника:\n" + employeeQuery.lastError().text());
            }
        }
        else if (roleId == 0) { // Аутентификация не прошла
            QMessageBox::warning(this, "Ошибка", "Неверный логин или пароль");
        }
        else {
            QMessageBox::warning(this, "Ошибка", "Неизвестная роль пользователя");
        }
    } else {
        // Ошибка аутентификации
        QSqlError error = query.lastError();
        QMessageBox::critical(this, "Ошибка входа",
                              "Неверный логин или пароль.\n"
                              "Подробности: " + error.text());
    }
}

void LoginWidget::on_startRegisterButton_clicked()
{
    RegistrationWidget *regWidget = new RegistrationWidget();
    regWidget->show();
    regWidget->setAttribute(Qt::WA_DeleteOnClose);
    this->close(); // Закрываем окно входа
}
