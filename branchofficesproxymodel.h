#ifndef BRANCHOFFICESPROXYMODEL_H
#define BRANCHOFFICESPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QString>

// Прокси-модель для фильтрации и сортировки
class BranchOfficesProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit BranchOfficesProxyModel(QObject *parent = nullptr);

    // Установка фильтров
    void setCityFilter(const QString &city);
    void setNameFilter(const QString &name);

    // Сброс фильтров
    void clearFilters();

protected:
    // Метод фильтрации строк
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString cityFilter;
    QString nameFilter;
};

#endif // BRANCHOFFICESPROXYMODEL_H
