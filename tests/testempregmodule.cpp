#include "testempregmodule.h"
#include "../empreg.h"
#include <QTest>

void TestEmpRegModule::testEmpRegConstructor()
{
    EmpReg empReg;
    QVERIFY(empReg.isEnabled());
    QVERIFY(empReg.size().isValid());
}

void TestEmpRegModule::testEmpRegSizeInitialization()
{
    EmpReg empReg;
    QSize size = empReg.size();
    QVERIFY(size.isValid());
}
