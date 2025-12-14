#ifndef EMPMAINWINDOW_H
#define EMPMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class EmpMainWindow;
}

class EmpMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EmpMainWindow(QWidget *parent = nullptr);
    ~EmpMainWindow();

private:
    Ui::EmpMainWindow *ui;
};

#endif // EMPMAINWINDOW_H
