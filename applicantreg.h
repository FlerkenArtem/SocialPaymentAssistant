#ifndef APPLICANTREG_H
#define APPLICANTREG_H

#include <QWidget>

namespace Ui {
class ApplicantReg;
}

class ApplicantReg : public QWidget
{
    Q_OBJECT

public:
    explicit ApplicantReg(QWidget *parent = nullptr);
    ~ApplicantReg();

private:
    Ui::ApplicantReg *ui;
};

#endif // APPLICANTREG_H
