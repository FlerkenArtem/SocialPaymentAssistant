#ifndef TESTLOGINWIDGET_WHITEBOX_H
#define TESTLOGINWIDGET_WHITEBOX_H

#include <QtTest/QtTest>

class TestLoginWidgetWhiteBox : public QObject
{
    Q_OBJECT

private slots:
    void testConstructorBehavior();
    void testWindowVisibilityAfterInit();
    void testWidgetSizeInitialization();
    void testDefaultWindowFlags();
private:
    QTEST_MAIN(TestLoginWidgetWhiteBox)
#include <QtTest/QtTest>
};

#endif
