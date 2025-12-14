#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QSqlQuery>

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

private slots:
    void on_endLoginButton_clicked();
    void on_startRegisterButton_clicked();

private:
    Ui::LoginWidget *ui;
};

#endif // LOGINWIDGET_H
