#ifndef TESTEMPREG_MODULE_H
#define TESTEMPREG_MODULE_H

#include <QtTest/QtTest>


class TestEmpRegModule : public QObject
{
    Q_OBJECT

private slots:
    void testEmpRegConstructor();
    void testEmpRegSizeInitialization();

private:
    QTEST_MAIN(TestEmpRegModule)
#include <QtTest/QtTest>
};

#endif
