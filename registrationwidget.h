#ifndef REGISTRATIONWIDGET_H
#define REGISTRATIONWIDGET_H

#include <QWidget>
#include <QMessageBox>

namespace Ui {
class RegistrationWidget;
}

class RegistrationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegistrationWidget(QWidget *parent = nullptr);
    ~RegistrationWidget();

private slots:
    void on_loginButton_clicked();      // Кнопка "Войти"
    void on_nextButton_clicked();       // Кнопка "Продолжить регистрацию"

private:
    Ui::RegistrationWidget *ui;
};

#endif // REGISTRATIONWIDGET_H
