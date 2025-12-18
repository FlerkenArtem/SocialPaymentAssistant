#ifndef TESTLOGINWIDGET_BLACKBOX_H
#define TESTLOGINWIDGET_BLACKBOX_H

#include <QtTest/QtTest>


class TestLoginWidgetBlackBox : public QObject
{
    Q_OBJECT

private slots:
    void testWindowBehavior();
    void testShowHideCycle();
    void testActivationState();
    void testResizeBehavior();

private:
    QTEST_MAIN(TestLoginWidgetBlackBox)
#include <QtTest/QtTest>

};

#endif
