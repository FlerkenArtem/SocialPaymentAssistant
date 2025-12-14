#include "branchofficeslist.h"
#include "ui_branchofficeslist.h"

BranchOfficesList::BranchOfficesList(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BranchOfficesList)
{
    ui->setupUi(this);
}

BranchOfficesList::~BranchOfficesList()
{
    delete ui;
}
