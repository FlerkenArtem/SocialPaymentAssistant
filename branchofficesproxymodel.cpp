#include "branchofficesproxymodel.h"

BranchOfficesProxyModel::BranchOfficesProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void BranchOfficesProxyModel::setCityFilter(const QString &city)
{
    cityFilter = city;
    invalidateFilter();
}

void BranchOfficesProxyModel::setNameFilter(const QString &name)
{
    nameFilter = name;
    invalidateFilter();
}

void BranchOfficesProxyModel::clearFilters()
{
    cityFilter.clear();
    nameFilter.clear();
    invalidateFilter();
}

bool BranchOfficesProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!sourceModel())
        return false;

    // Получаем индексы для столбцов города и названия
    QModelIndex cityIndex = sourceModel()->index(source_row, 2, source_parent);
    QModelIndex nameIndex = sourceModel()->index(source_row, 1, source_parent);

    QString city = sourceModel()->data(cityIndex).toString();
    QString name = sourceModel()->data(nameIndex).toString();

    // Проверяем соответствие фильтрам
    bool cityMatch = cityFilter.isEmpty() || city.contains(cityFilter, Qt::CaseInsensitive);
    bool nameMatch = nameFilter.isEmpty() || name.contains(nameFilter, Qt::CaseInsensitive);

    return cityMatch && nameMatch;
}
