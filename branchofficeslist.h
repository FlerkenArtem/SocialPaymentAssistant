#ifndef BRANCHOFFICESLIST_H
#define BRANCHOFFICESLIST_H

#include <QWidget>
#include "branchofficeslistmodel.h"
#include "branchofficesproxymodel.h"

namespace Ui {
class BranchOfficesList;
}

class BranchOfficesList : public QWidget
{
    Q_OBJECT

public:
    explicit BranchOfficesList(QWidget *parent = nullptr);
    ~BranchOfficesList();

    // Метод для обновления данных
    void refreshData();

private slots:
    void on_acceptBranchOfficeFilterButton_clicked();
    void on_branchOfficeClose_clicked();

private:
    void setupTableView();
    void loadCitiesToComboBox();

    Ui::BranchOfficesList *ui;
    BranchOfficesListModel *model;
    BranchOfficesProxyModel *proxyModel;
};

#endif // BRANCHOFFICESLIST_H
