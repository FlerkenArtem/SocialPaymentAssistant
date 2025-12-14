#include "registrationwidget.h"
#include "ui_registrationwidget.h"
#include "empdatainsert.h"
#include "applicantdatainsert.h"
#include "loginwidget.h"
#include <QSqlQuery>
#include <QSqlError>

RegistrationWidget::RegistrationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegistrationWidget)
{
    ui->setupUi(this);

    // Подключаем кнопки к слотам
    connect(ui->loginButton, &QPushButton::clicked, this, &RegistrationWidget::on_loginButton_clicked);
    connect(ui->nextButton, &QPushButton::clicked, this, &RegistrationWidget::on_nextButton_clicked);
}

RegistrationWidget::~RegistrationWidget()
{
    delete ui;
}

void RegistrationWidget::on_loginButton_clicked()
{
    // Возвращаемся к окну входа
    LoginWidget *loginWidget = new LoginWidget();
    loginWidget->show();
    this->close();
}

void RegistrationWidget::on_nextButton_clicked()
{
    // Проверяем, заполнены ли обязательные поля
    QString login = ui->newLogin->text();
    QString email = ui->newEmail->text();
    QString password = ui->newPassword->text();
    QString repeatPassword = ui->repeatPassword->text();

    if (login.isEmpty() || email.isEmpty() || password.isEmpty() || repeatPassword.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Все поля обязательны для заполнения");
        return;
    }

    // Проверяем совпадение паролей
    if (password != repeatPassword) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают");
        return;
    }

    // Проверяем, выбрана ли роль
    if (!ui->empRadioButton->isChecked() && !ui->applicantRadioButton->isChecked()) {
        QMessageBox::warning(this, "Ошибка", "Выберите тип аккаунта");
        return;
    }

    // В зависимости от выбранной роли открываем соответствующее окно для заполнения данных
    if (ui->empRadioButton->isChecked()) {
        // Открываем окно для заполнения данных сотрудника с передачей учетных данных
        EmpDataInsert *empDataInsert = new EmpDataInsert(login, email, password);
        empDataInsert->show();
        this->close();
    } else if (ui->applicantRadioButton->isChecked()) {
        // Открываем окно для заполнения данных заявителя
        ApplicantDataInsert *applicantDataInsert = new ApplicantDataInsert();
        applicantDataInsert->show();
        this->close();
    }
}
