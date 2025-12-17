#include "branchofficeslist.h"
#include "ui_branchofficeslist.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QHeaderView>

BranchOfficesList::BranchOfficesList(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BranchOfficesList)
    , model(new BranchOfficesListModel(this))
    , proxyModel(new BranchOfficesProxyModel(this))
{
    ui->setupUi(this);

    setupTableView();
    loadCitiesToComboBox();
}

BranchOfficesList::~BranchOfficesList()
{
    delete ui;
}

void BranchOfficesList::refreshData()
{
    model->updateData();
}

void BranchOfficesList::on_acceptBranchOfficeFilterButton_clicked()
{
    QString city = ui->cityComboBox->currentText();
    QString name = ui->lineEdit->text().trimmed();

    // Если выбран "Все города", очищаем фильтр
    if (city == "Все города") {
        proxyModel->setCityFilter("");
    } else {
        proxyModel->setCityFilter(city);
    }

    proxyModel->setNameFilter(name);
}

void BranchOfficesList::on_branchOfficeClose_clicked()
{
    this->close();
}

void BranchOfficesList::setupTableView()
{
    // Настройка прокси-модели и таблицы
    proxyModel->setSourceModel(model);
    ui->branchOfficesTableView->setModel(proxyModel);

    // Включаем сортировку
    ui->branchOfficesTableView->setSortingEnabled(true);

    // Настройка поведения выделения
    ui->branchOfficesTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->branchOfficesTableView->setSelectionMode(QAbstractItemView::SingleSelection);

    // Настройка заголовков
    ui->branchOfficesTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->branchOfficesTableView->horizontalHeader()->setStretchLastSection(true);

    // Автоматическое изменение размера столбцов по содержимому
    ui->branchOfficesTableView->resizeColumnsToContents();

    // Скрываем ID колонку, если не нужна
    ui->branchOfficesTableView->setColumnHidden(0, true);
}

void BranchOfficesList::loadCitiesToComboBox()
{
    ui->cityComboBox->clear();
    ui->cityComboBox->addItem("Все города");

    QSqlQuery query;
    query.prepare("SELECT DISTINCT name_city FROM city ORDER BY name_city");

    if (query.exec()) {
        while (query.next()) {
            ui->cityComboBox->addItem(query.value(0).toString());
        }
    } else {
        qDebug() << "Ошибка загрузки городов:" << query.lastError().text();
    }
}
