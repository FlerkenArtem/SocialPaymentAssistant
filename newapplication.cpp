#include "newapplication.h"
#include "ui_newapplication.h"

NewApplication::NewApplication(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NewApplication)
{
    ui->setupUi(this);
}

NewApplication::~NewApplication()
{
    delete ui;
}
