#include "policyedit.h"
#include "ui_policyedit.h"

PolicyEdit::PolicyEdit(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PolicyEdit)
{
    ui->setupUi(this);
}

PolicyEdit::~PolicyEdit()
{
    delete ui;
}
