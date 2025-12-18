#include "newapplication.h"
#include "qsqlerror.h"
#include "ui_newapplication.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>

NewApplication::NewApplication(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NewApplication)
    , m_applicantId(-1) // Инициализируем недопустимым значением
{
    ui->setupUi(this);

    QSqlQuery query("SELECT type_name FROM TYPE_OF_SOCIAL_PAYMENT ORDER BY type_name");
    while (query.next()) {
        ui->typeComboBox->addItem(query.value(0).toString());
    }
}

NewApplication::~NewApplication()
{
    delete ui;
}

void NewApplication::setApplicantId(int applicantId)
{
    m_applicantId = applicantId;
}

void NewApplication::on_newApplicationOK_clicked()
{
    // Проверяем, установлен ли applicant_id
    if (m_applicantId == -1) {
        QMessageBox::warning(this, "Ошибка", "Не установлен ID пользователя.");
        return;
    }

    // Получаем данные из формы
    QString typeName = ui->typeComboBox->currentText();
    double amount = ui->amountEdit->value();

    if (typeName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите тип заявки.");
        return;
    }

    if (amount <= 0) {
        QMessageBox::warning(this, "Ошибка", "Сумма должна быть больше 0.");
        return;
    }

    // Вызываем хранимую процедуру new_application
    QSqlQuery query;
    query.prepare("CALL new_application(:applicant_id, :type_name, :amount)");

    // Используем bindValue для привязки параметров
    query.bindValue(":applicant_id", m_applicantId);
    query.bindValue(":type_name", typeName);
    query.bindValue(":amount", amount);

    if (query.exec()) {
        QMessageBox::information(this, "Успех", "Заявка успешно создана.");
        this->close(); // Закрываем окно после успешного создания
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось создать заявку: " + query.lastError().text());
        qDebug() << "Ошибка выполнения запроса:" << query.lastError();
    }
}

void NewApplication::on_newApplicationCancel_clicked()
{
    this->close(); // Закрываем окно без сохранения
}
