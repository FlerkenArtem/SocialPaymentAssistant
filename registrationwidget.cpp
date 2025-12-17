#include "registrationwidget.h"
#include "ui_registrationwidget.h"
#include "empreg.h"
#include "applicantreg.h"
#include "loginwidget.h"

RegistrationWidget::RegistrationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegistrationWidget)
{
    ui->setupUi(this);

    // Устанавливаем начальное состояние кнопки "Продолжить"
    updateNextButtonState();

    // Подключаем радио-кнопки для обновления состояния
    connect(ui->empRadioButton, &QRadioButton::toggled, this, &RegistrationWidget::updateNextButtonState);
    connect(ui->applicantRadioButton, &QRadioButton::toggled, this, &RegistrationWidget::updateNextButtonState);
}

RegistrationWidget::~RegistrationWidget()
{
    delete ui;
}

void RegistrationWidget::on_loginButton_clicked()
{
    // Переход к окну входа
    LoginWidget *loginWidget = new LoginWidget();
    loginWidget->show();
    this->close();
}

void RegistrationWidget::on_nextButton_clicked()
{
    // Проверяем, какая роль выбрана
    if (ui->empRadioButton->isChecked()) {
        // Открываем окно регистрации сотрудника
        EmpReg *empReg = new EmpReg();
        empReg->show();
        this->close();
    }
    else if (ui->applicantRadioButton->isChecked()) {
        // Открываем окно регистрации заявителя
        ApplicantReg *applicantReg = new ApplicantReg();
        applicantReg->show();
        this->close();
    }
    else {
        // Ничего не выбрано - показываем предупреждение
        QMessageBox::warning(this, "Выбор типа аккаунта",
                             "Пожалуйста, выберите тип аккаунта для регистрации.");
    }
}

void RegistrationWidget::updateNextButtonState()
{
    // Активируем кнопку "Продолжить", только если выбрана одна из ролей
    bool hasSelection = ui->empRadioButton->isChecked() ||
                        ui->applicantRadioButton->isChecked();
    ui->nextButton->setEnabled(hasSelection);
}
