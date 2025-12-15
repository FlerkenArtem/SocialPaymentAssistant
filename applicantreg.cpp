#include "applicantreg.h"
#include "ui_applicantreg.h"

ApplicantReg::ApplicantReg(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ApplicantReg)
{
    ui->setupUi(this);
}

ApplicantReg::~ApplicantReg()
{
    delete ui;
}
