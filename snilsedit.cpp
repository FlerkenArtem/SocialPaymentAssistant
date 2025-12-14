#include "snilsedit.h"
#include "ui_snilsedit.h"

SnilsEdit::SnilsEdit(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SnilsEdit)
{
    ui->setupUi(this);
}

SnilsEdit::~SnilsEdit()
{
    delete ui;
}
