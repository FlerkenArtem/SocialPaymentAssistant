#include "testloginwidgetblackbox.h"
#include "../loginwidget.h"

void TestLoginWidgetBlackBox::testWindowBehavior()
{
    LoginWidget login;

    // Черный ящик: поведение окна
    QVERIFY(!login.isVisible());
    login.show();
    QTRY_VERIFY(login.isVisible());
}

void TestLoginWidgetBlackBox::testShowHideCycle()
{
    LoginWidget login;
    login.show();
    QTRY_VERIFY(login.isVisible());

    login.hide();
    QTRY_VERIFY(!login.isVisible());
}

void TestLoginWidgetBlackBox::testActivationState()
{
    LoginWidget login;
    login.show();

    QTRY_VERIFY(login.isActiveWindow() || login.isEnabled());
}

void TestLoginWidgetBlackBox::testResizeBehavior()
{
    LoginWidget login;
    login.resize(400, 300);

    QCOMPARE(login.size(), QSize(400, 300));
}
