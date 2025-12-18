#ifndef TESTLOAD_H
#define TESTLOAD_H

#include <QtTest/QtTest>


class TestLoad : public QObject
{
    Q_OBJECT

private slots:
    void testMassWidgetCreation();
    void testWidgetResizePerformance();

private:
    QTEST_MAIN(TestLoad)
#include <QtTest/QtTest>
};

#endif
