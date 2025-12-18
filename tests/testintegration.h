#ifndef TESTINTEGRATION_H
#define TESTINTEGRATION_H

#include <QtTest/QtTest>


class TestIntegration : public QObject
{
    Q_OBJECT

private slots:
    void testWidgetCreationOrder();
    void testMemoryManagement();
    void testApplicationWidgetCount();

private:
    QTEST_MAIN(TestIntegration)
#include <QtTest/QtTest>
};

#endif
