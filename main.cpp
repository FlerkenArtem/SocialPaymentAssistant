#include <QApplication>
#include <loginwidget.h>
#include <QSqlDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Подключаем БД
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("social_payments_assistant");
    db.setUserName("postgres");
    db.setPassword("postgres");
    bool ok = db.open();
    qDebug() << "db = " << ok;

    // Создаем виджет входа в приложение
    LoginWidget* login = new LoginWidget();
    login->show();
    login->setAttribute(Qt::WA_DeleteOnClose);
    return a.exec();
}
