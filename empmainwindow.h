#ifndef EMPMAINWINDOW_H
#define EMPMAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include "empapplicationsmodel.h"

namespace Ui {
class EmpMainWindow;
}

class EmpMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EmpMainWindow(int employeeId, QWidget *parent = nullptr);
    ~EmpMainWindow();

private slots:
    void on_open_clicked();
    void on_cancel_clicked();
    void on_accept_clicked();

    // Меню
    void on_empLogoutAct_triggered();
    void on_empFullscreenAct_triggered();
    void on_empQuitAct_triggered();
    void on_empAboutAct_triggered();

    void refreshApplicationData();

private:
    Ui::EmpMainWindow *ui;
    int m_employeeId;
    EmpApplicationsModel *m_applicationsModel;
    int m_currentApplicationId;

    void showApplicationDetails(int applicationId);
    void generateAndSaveCertificate(int applicationId, double requestedAmount, double approvedAmount);
};

#endif // EMPMAINWINDOW_H
