#include "empreg.h"
#include "ui_empreg.h"

EmpReg::EmpReg(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EmpReg)
{
    ui->setupUi(this);
}

EmpReg::~EmpReg()
{
    delete ui;
}
