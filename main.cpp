// #include "tests/testempregmodule.h"
// #include "tests/testintegration.h"
// #include "tests/testload.h"
// #include "tests/testloginwidgetblackbox.h"
// #include "tests/testloginwidgetwhitebox.h"
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

    // // Тестирование
    // qDebug() << "SocialPaymentsAssistant Tests";
    // qDebug() << "DateTime: " << QDateTime::currentDateTime().toLocalTime().toString();

    // TestLoginWidgetWhiteBox whiteBox;
    // TestLoginWidgetBlackBox blackBox;
    // TestEmpRegModule moduleTest;
    // TestIntegration integrationTest;
    // TestLoad loadTest;

    // int result = 0;

    // qDebug() << "\n1. White Box:";
    // result += QTest::qExec(&whiteBox, argc, argv);

    // qDebug() << "\n2. Black Box:";
    // result += QTest::qExec(&blackBox, argc, argv);

    // qDebug() << "\n3. Unit:";
    // result += QTest::qExec(&moduleTest, argc, argv);

    // qDebug() << "\n4. Integration:";
    // result += QTest::qExec(&integrationTest, argc, argv);

    // qDebug() << "\n5. Load:";
    // result += QTest::qExec(&loadTest, argc, argv);

    // qDebug() << "\nErrors: " << result;


    // Создаем виджет входа в приложение
    LoginWidget* login = new LoginWidget();
    login->show();
    login->setAttribute(Qt::WA_DeleteOnClose);
    return a.exec();
}
