#ifndef BRANCHOFFICESLIST_H
#define BRANCHOFFICESLIST_H

#include <QWidget>

namespace Ui {
class BranchOfficesList;
}

class BranchOfficesList : public QWidget
{
    Q_OBJECT

public:
    explicit BranchOfficesList(QWidget *parent = nullptr);
    ~BranchOfficesList();

private:
    Ui::BranchOfficesList *ui;
};

#endif // BRANCHOFFICESLIST_H
