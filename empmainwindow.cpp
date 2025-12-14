#include "empmainwindow.h"
#include "ui_empmainwindow.h"

EmpMainWindow::EmpMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EmpMainWindow)
{
    ui->setupUi(this);
}

EmpMainWindow::~EmpMainWindow()
{
    delete ui;
}
