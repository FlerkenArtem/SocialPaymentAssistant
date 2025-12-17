#include "loginwidget.h"
#include "ui_loginwidget.h"
#include "registrationwidget.h"
#include "mainwindow.h"
#include "empmainwindow.h"
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
            MainWindow *mainWindow = new MainWindow();
            mainWindow->show();
            this->close(); // Закрываем окно входа
        } else if (roleId == 2) { // Сотрудник
            EmpMainWindow *empWindow = new EmpMainWindow();
            empWindow->show();
            this->close(); // Закрываем окно входа
        } else {
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
