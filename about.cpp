#include "about.h"
#include "ui_about.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <QDebug>

About::About(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::About)
{
    ui->setupUi(this);
    loadReadme();
}

About::~About()
{
    delete ui;
}

void About::loadReadme()
{
    QString content = "Интерактивный помощник для составления заявлений на получение социальных выплат\n"
                      "Основная информация:\n\n"
                      "1. Система для подачи заявлений на социальные выплаты\n"
                      "2. Разработана на C++ с использованием Qt 6.9.3\n"
                      "3. Использует PostgreSQL в качестве базы данных\n\n"
                      "Авторы:\n"
                      "Хромов А.Е., Горбунова Е.А., Карташова В.А., Сафонова Д.А., Тылик А.В.\n"
                      "Научный руководитель: Левшин С.С.\n\n"
                      "ВГТУ, СПК, ИСП-234о.\n"
                      "Проект создан в учебных целях.";


    // Настраиваем QTextBrowser
    ui->aboutText->setText(content);
    ui->aboutText->setReadOnly(true);
    ui->aboutText->setStyleSheet("background-color: white; color: black;");
}

void About::on_aboutCloseButton_clicked()
{
    this->close();
}
