#ifndef EMPREG_H
#define EMPREG_H

#include <QWidget>

namespace Ui {
class EmpReg;
}

class EmpReg : public QWidget
{
    Q_OBJECT

public:
    explicit EmpReg(QWidget *parent = nullptr);
    ~EmpReg();

private:
    Ui::EmpReg *ui;
};

#endif // EMPREG_H
