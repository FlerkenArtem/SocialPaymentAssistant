#include "mainwindow.h"
#include "branchofficeslist.h"
#include "ui_mainwindow.h"
#include "newapplication.h"
#include "about.h"
#include "loginwidget.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextBrowser>
#include <QTabWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(int applicantId, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_applicantId(applicantId)
{
    ui->setupUi(this);

    // Инициализация моделей
    m_myApplicationsModel = new MyApplicationsModel(m_applicantId, this);
    m_acceptedApplicationsModel = new AcceptedApplicationsModel(m_applicantId, this);

    // Инициализация прокси-моделей
    m_myApplicationsProxy = new ApplicationsProxyModel(this);
    m_myApplicationsProxy->setSourceModel(m_myApplicationsModel);

    m_acceptedApplicationsProxy = new ApplicationsProxyModel(this);
    m_acceptedApplicationsProxy->setSourceModel(m_acceptedApplicationsModel);

    // Настройка TableView
    ui->myApplicationsTableView->setModel(m_myApplicationsProxy);
    ui->myApplicationsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->myApplicationsTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->myApplicationsTableView->setColumnHidden(0, true); // Скрываем ID

    ui->acceptedApplicationsTableView->setModel(m_acceptedApplicationsProxy);
    ui->acceptedApplicationsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->acceptedApplicationsTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->acceptedApplicationsTableView->setColumnHidden(0, true); // Скрываем ID

    // Загрузка фильтров
    loadPaymentTypes();
    loadStatuses();

    // Настройка дат
    ui->myApplicationDateEdit->setDate(QDate::currentDate());
    ui->acceptDateEdit->setDate(QDate::currentDate());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadPaymentTypes()
{
    ui->myApplicationTypeEdit->clear();
    ui->paymentTypeComboBox->clear();

    ui->myApplicationTypeEdit->addItem("Все");
    ui->paymentTypeComboBox->addItem("Все");

    QSqlQuery query("SELECT type_name FROM type_of_social_payment ORDER BY type_name");
    while (query.next()) {
        QString type = query.value(0).toString();
        ui->myApplicationTypeEdit->addItem(type);
        ui->paymentTypeComboBox->addItem(type);
    }
}

void MainWindow::loadStatuses()
{
    ui->myApplicationsStatusComboBox->clear();
    ui->myApplicationsStatusComboBox->addItem("Все");

    QSqlQuery query("SELECT status FROM application_status ORDER BY status");
    while (query.next()) {
        ui->myApplicationsStatusComboBox->addItem(query.value(0).toString());
    }
}

void MainWindow::refreshModels()
{
    m_myApplicationsModel->refreshData();
    m_acceptedApplicationsModel->refreshData();
}

void MainWindow::on_myApplicationFilterOK_clicked()
{
    QString paymentType = ui->myApplicationTypeEdit->currentText();
    QString status = ui->myApplicationsStatusComboBox->currentText();
    QDate date = ui->myApplicationDateEdit->date();

    m_myApplicationsProxy->setPaymentTypeFilter(paymentType);
    m_myApplicationsProxy->setStatusFilter(status);
    m_myApplicationsProxy->setDateFilter(date);
}

void MainWindow::on_acceptedApplicationFilterOK_clicked()
{
    QString paymentType = ui->paymentTypeComboBox->currentText();
    QDate date = ui->acceptDateEdit->date();

    m_acceptedApplicationsProxy->setPaymentTypeFilter(paymentType);
    m_acceptedApplicationsProxy->setAcceptanceDateFilter(date);
}

void MainWindow::on_showApplicationDoc_clicked()
{
    QModelIndexList selected = ui->acceptedApplicationsTableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите заявку для просмотра");
        return;
    }

    int sourceRow = m_acceptedApplicationsProxy->mapToSource(selected.first()).row();
    AcceptedApplicationData app = m_acceptedApplicationsModel->getApplicationData(sourceRow);

    // Формируем информацию для отображения
    QString content = QString(
                          "<h2>Детали заявки #%1</h2>"
                          "<table border='1' cellpadding='5'>"
                          "<tr><td><b>ID заявки:</b></td><td>%1</td></tr>"
                          "<tr><td><b>Тип выплаты:</b></td><td>%2</td></tr>"
                          "<tr><td><b>Дата принятия:</b></td><td>%3</td></tr>"
                          "<tr><td><b>Сумма:</b></td><td>%4 руб.</td></tr>"
                          "<tr><td><b>Файл справки:</b></td><td>%5</td></tr>"
                          "</table>"
                          "<p><i>Для сохранения документа нажмите кнопку 'Сохранить как'</i></p>"
                          ).arg(app.applicationId)
                          .arg(app.paymentType)
                          .arg(app.acceptanceDate.toString("dd.MM.yyyy"))
                          .arg(QString::number(app.amount, 'f', 2))
                          .arg(app.certificateFileName.isEmpty() ? "Не указан" : app.certificateFileName);

    showDocumentViewTab("Просмотр заявки #" + QString::number(app.applicationId), content);
}

void MainWindow::on_saveApplicationDoc_clicked()
{
    QModelIndexList selected = ui->acceptedApplicationsTableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите заявку для сохранения");
        return;
    }

    int sourceRow = m_acceptedApplicationsProxy->mapToSource(selected.first()).row();
    AcceptedApplicationData app = m_acceptedApplicationsModel->getApplicationData(sourceRow);

    if (app.certificateData.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Документ отсутствует");
        return;
    }

    QString defaultFileName = QString("Справка_%1_%2.pdf")
                                  .arg(app.applicationId)
                                  .arg(QDate::currentDate().toString("ddMMyyyy"));

    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Сохранить документ",
        QDir::homePath() + "/" + defaultFileName,
        "Документы PDF (*.pdf);;Все файлы (*.*)"
        );

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(app.certificateData);
            file.close();
            QMessageBox::information(this, "Успех", "Документ сохранен");
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось сохранить файл");
        }
    }
}

void MainWindow::on_newApplicationAct_triggered()
{
    NewApplication *newAppWidget = new NewApplication();
    newAppWidget->setAttribute(Qt::WA_DeleteOnClose);
    newAppWidget->setApplicantId(m_applicantId);
    int tabIndex = ui->tabWidget->addTab(newAppWidget, "Новая заявка");
    ui->tabWidget->setCurrentIndex(tabIndex);
    connect(newAppWidget, &NewApplication::destroyed, [this, tabIndex]() {
        ui->tabWidget->removeTab(tabIndex);
        refreshModels(); // Обновляем модели после создания заявки
    });
}

void MainWindow::on_myApplicationsAct_triggered()
{
    ui->tabWidget->setCurrentWidget(ui->myApplicationsTab);
}

void MainWindow::on_acceptedApplicationsAct_triggered()
{
    ui->tabWidget->setCurrentWidget(ui->acceptedApplications);
}

void MainWindow::on_logoutAct_triggered()
{
    LoginWidget *login = new LoginWidget();
    login->show();
    this->close();
}

void MainWindow::on_fullscreenAct_triggered()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

void MainWindow::on_exitAct_triggered()
{
    qApp->quit();
}

void MainWindow::on_branchOfficesAct_triggered()
{
    showBranchOfficesTab();
}

void MainWindow::on_aboutAct_triggered()
{
    showAboutTab();
}

void MainWindow::showDocumentViewTab(const QString &title, const QString &content)
{
    QWidget *docViewWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(docViewWidget);

    QTextBrowser *textBrowser = new QTextBrowser();
    textBrowser->setHtml(content);
    layout->addWidget(textBrowser);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *closeButton = new QPushButton("Закрыть");
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    layout->addLayout(buttonLayout);

    int tabIndex = ui->tabWidget->addTab(docViewWidget, title);
    ui->tabWidget->setCurrentIndex(tabIndex);

    connect(closeButton, &QPushButton::clicked, [this, tabIndex]() {
        ui->tabWidget->removeTab(tabIndex);
    });
}

void MainWindow::showBranchOfficesTab()
{
    BranchOfficesList *branchWidget = new BranchOfficesList();
    branchWidget->setAttribute(Qt::WA_DeleteOnClose);
    int tabIndex = ui->tabWidget->addTab(branchWidget, "Филиалы");
    ui->tabWidget->setCurrentIndex(tabIndex);


    connect(branchWidget, &BranchOfficesList::destroyed, [this, tabIndex]() {
        ui->tabWidget->removeTab(tabIndex);
    });
}

void MainWindow::showAboutTab()
{
    About *aboutWidget = new About();
    aboutWidget->setAttribute(Qt::WA_DeleteOnClose);
    int tabIndex = ui->tabWidget->addTab(aboutWidget, "О программе");
    ui->tabWidget->setCurrentIndex(tabIndex);

    connect(aboutWidget, &About::destroyed, [this, tabIndex]() {
        ui->tabWidget->removeTab(tabIndex);
    });
}
