#ifndef NEWAPPLICATION_H
#define NEWAPPLICATION_H

#include <QWidget>
#include <QSqlDatabase>

namespace Ui {
class NewApplication;
}

class NewApplication : public QWidget
{
    Q_OBJECT

public:
    explicit NewApplication(QWidget *parent = nullptr);
    ~NewApplication();

    // Установка ID текущего пользователя
    void setApplicantId(int applicantId);

private slots:
    // Слоты для кнопок
    void on_newApplicationOK_clicked();
    void on_newApplicationCancel_clicked();

private:
    Ui::NewApplication *ui;
    int m_applicantId; // ID вошедшего пользователя
};

#endif // NEWAPPLICATION_H
