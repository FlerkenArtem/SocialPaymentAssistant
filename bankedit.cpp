#include "bankedit.h"
#include "ui_bankedit.h"

BankEdit::BankEdit(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BankEdit)
{
    ui->setupUi(this);
}

BankEdit::~BankEdit()
{
    delete ui;
}
