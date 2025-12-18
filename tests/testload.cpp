#include "testload.h"
#include "../loginwidget.h"
#include <QElapsedTimer>

void TestLoad::testMassWidgetCreation()
{
    const int count = 100;
    QElapsedTimer timer;
    timer.start();

    QList<LoginWidget*> widgets;
    for (int i = 0; i < count; ++i) {
        LoginWidget* w = new LoginWidget();
        widgets.append(w);
    }

    qDeleteAll(widgets); // Очищаем все разом

    qint64 elapsed = timer.elapsed();
    qDebug() << "100 LoginWidget:" << elapsed << "ms";
    QVERIFY2(elapsed < 3000, "Создание слишком медленное");
}


void TestLoad::testWidgetResizePerformance()
{
    const int count = 1000;
    QElapsedTimer timer;
    timer.start();

    LoginWidget login;
    for (int i = 0; i < count; ++i) {
        login.resize(300 + i % 100, 200 + i % 50);
    }

    qint64 elapsed = timer.elapsed();
    QVERIFY2(elapsed < 500, "Resize слишком медленный");
}
