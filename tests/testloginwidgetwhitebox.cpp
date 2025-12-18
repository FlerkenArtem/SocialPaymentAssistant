#include "testloginwidgetwhitebox.h"
#include "../loginwidget.h"

void TestLoginWidgetWhiteBox::testConstructorBehavior()
{
    LoginWidget login;

    // Белый ящик: проверяем инициализацию конструктора
    QVERIFY(login.isVisible() == false);
    QVERIFY(login.isEnabled());
}

void TestLoginWidgetWhiteBox::testWindowVisibilityAfterInit()
{
    LoginWidget login;
    login.show();

    QTRY_VERIFY(login.isVisible());
}

void TestLoginWidgetWhiteBox::testWidgetSizeInitialization()
{
    LoginWidget login;

    QSize size = login.size();
    QVERIFY(size.width() > 0);
    QVERIFY(size.height() > 0);
}

void TestLoginWidgetWhiteBox::testDefaultWindowFlags()
{
    LoginWidget login;

    Qt::WindowFlags flags = login.windowFlags();
    QVERIFY((flags & Qt::Window) != 0);
}

