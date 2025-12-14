#ifndef ADDRESSEDIT_H
#define ADDRESSEDIT_H

#include <QWidget>

namespace Ui {
class AddressEdit;
}

class AddressEdit : public QWidget
{
    Q_OBJECT

public:
    explicit AddressEdit(QWidget *parent = nullptr);
    ~AddressEdit();

private:
    Ui::AddressEdit *ui;
};

#endif // ADDRESSEDIT_H
