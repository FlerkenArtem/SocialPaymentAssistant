#ifndef NEWAPPLICATION_H
#define NEWAPPLICATION_H

#include <QWidget>

namespace Ui {
class NewApplication;
}

class NewApplication : public QWidget
{
    Q_OBJECT

public:
    explicit NewApplication(QWidget *parent = nullptr);
    ~NewApplication();

private:
    Ui::NewApplication *ui;
};

#endif // NEWAPPLICATION_H
