#ifndef SNILSEDIT_H
#define SNILSEDIT_H

#include <QWidget>

namespace Ui {
class SnilsEdit;
}

class SnilsEdit : public QWidget
{
    Q_OBJECT

public:
    explicit SnilsEdit(QWidget *parent = nullptr);
    ~SnilsEdit();

private:
    Ui::SnilsEdit *ui;
};

#endif // SNILSEDIT_H
