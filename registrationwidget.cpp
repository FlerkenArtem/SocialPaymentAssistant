#include "registrationwidget.h"
#include "ui_registrationwidget.h"
#include <QSqlQuery>
#include <QSqlError>

RegistrationWidget::RegistrationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegistrationWidget)
{
    ui->setupUi(this);
}

RegistrationWidget::~RegistrationWidget()
{
    delete ui;
}
