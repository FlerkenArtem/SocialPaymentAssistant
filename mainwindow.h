#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include "myapplicationsmodel.h"
#include "acceptedapplicationsmodel.h"
#include "applicationsproxymodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int applicantId, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_myApplicationFilterOK_clicked();
    void on_acceptedApplicationFilterOK_clicked();
    void on_showApplicationDoc_clicked();
    void on_saveApplicationDoc_clicked();

    void on_newApplicationAct_triggered();
    void on_myApplicationsAct_triggered();
    void on_acceptedApplicationsAct_triggered();
    void on_logoutAct_triggered();
    void on_fullscreenAct_triggered();
    void on_exitAct_triggered();
    void on_branchOfficesAct_triggered();
    void on_aboutAct_triggered();


private:
    Ui::MainWindow *ui;
    int m_applicantId;
    MyApplicationsModel *m_myApplicationsModel;
    AcceptedApplicationsModel *m_acceptedApplicationsModel;
    ApplicationsProxyModel *m_myApplicationsProxy;
    ApplicationsProxyModel *m_acceptedApplicationsProxy;

    void loadPaymentTypes();
    void loadStatuses();
    void refreshModels();
    void showDocumentViewTab(const QString &title, const QString &content);
    void showBranchOfficesTab();
    void showAboutTab();
};

#endif // MAINWINDOW_H
