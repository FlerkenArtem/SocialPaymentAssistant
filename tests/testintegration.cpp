#include "testintegration.h"
#include "../registrationwidget.h"
#include "../loginwidget.h"


void TestIntegration::testWidgetCreationOrder()
{
    LoginWidget login;
    RegistrationWidget reg;
    QVERIFY(login.isEnabled());
    QVERIFY(reg.isEnabled());
}

void TestIntegration::testMemoryManagement()
{
    int initialCount = QApplication::topLevelWidgets().size();

    {
        LoginWidget* login = new LoginWidget();
        QVERIFY(login != nullptr);
        delete login;
    }

    QTest::qWait(100);
    QVERIFY(QApplication::topLevelWidgets().size() == initialCount);
}

void TestIntegration::testApplicationWidgetCount()
{
    int initialCount = QApplication::topLevelWidgets().size();

    {
        LoginWidget login;
        RegistrationWidget reg;
        QCOMPARE(QApplication::topLevelWidgets().size(), initialCount + 2);
    }

    QCOMPARE(QApplication::topLevelWidgets().size(), initialCount);
}
