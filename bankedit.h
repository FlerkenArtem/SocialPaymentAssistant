#ifndef BANKEDIT_H
#define BANKEDIT_H

#include <QWidget>

namespace Ui {
class BankEdit;
}

class BankEdit : public QWidget
{
    Q_OBJECT

public:
    explicit BankEdit(QWidget *parent = nullptr);
    ~BankEdit();

private:
    Ui::BankEdit *ui;
};

#endif // BANKEDIT_H
