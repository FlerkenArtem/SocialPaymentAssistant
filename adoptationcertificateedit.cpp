#include "adoptationcertificateedit.h"
#include "ui_adoptationcertificateedit.h"

AdoptationCertificateEdit::AdoptationCertificateEdit(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdoptationCertificateEdit)
{
    ui->setupUi(this);
}

AdoptationCertificateEdit::~AdoptationCertificateEdit()
{
    delete ui;
}
