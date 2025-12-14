#include "applicantdatainsert.h"
#include "ui_applicantdatainsert.h"

ApplicantDataInsert::ApplicantDataInsert(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ApplicantDataInsert)
{
    ui->setupUi(this);
}

ApplicantDataInsert::~ApplicantDataInsert()
{
    delete ui;
}
