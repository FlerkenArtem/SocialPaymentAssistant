#include "addressedit.h"
#include "ui_addressedit.h"

AddressEdit::AddressEdit(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddressEdit)
{
    ui->setupUi(this);
}

AddressEdit::~AddressEdit()
{
    delete ui;
}
