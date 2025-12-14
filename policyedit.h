#ifndef POLICYEDIT_H
#define POLICYEDIT_H

#include <QWidget>

namespace Ui {
class PolicyEdit;
}

class PolicyEdit : public QWidget
{
    Q_OBJECT

public:
    explicit PolicyEdit(QWidget *parent = nullptr);
    ~PolicyEdit();

private:
    Ui::PolicyEdit *ui;
};

#endif // POLICYEDIT_H
