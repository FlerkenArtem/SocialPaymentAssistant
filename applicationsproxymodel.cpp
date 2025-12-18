#include "applicationsproxymodel.h"
#include <QDate>

ApplicationsProxyModel::ApplicationsProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void ApplicationsProxyModel::setPaymentTypeFilter(const QString &type)
{
    m_paymentTypeFilter = type;
    invalidateFilter();
}

void ApplicationsProxyModel::setStatusFilter(const QString &status)
{
    m_statusFilter = status;
    invalidateFilter();
}

void ApplicationsProxyModel::setDateFilter(const QDate &date)
{
    m_dateFilter = date;
    m_useDateFilter = date.isValid();
    invalidateFilter();
}

void ApplicationsProxyModel::setAcceptanceDateFilter(const QDate &date)
{
    m_acceptanceDateFilter = date;
    m_useAcceptanceDateFilter = date.isValid();
    invalidateFilter();
}

bool ApplicationsProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex typeIndex = sourceModel()->index(sourceRow, 1, sourceParent);
    QModelIndex statusIndex = sourceModel()->index(sourceRow, 2, sourceParent);
    QModelIndex dateIndex = sourceModel()->index(sourceRow, 3, sourceParent);
    QModelIndex acceptanceDateIndex = sourceModel()->index(sourceRow, 2, sourceParent); // Для принятых заявок

    QString type = sourceModel()->data(typeIndex).toString();
    QString status = sourceModel()->data(statusIndex).toString();
    QString dateStr = sourceModel()->data(dateIndex).toString();
    QString acceptanceDateStr = sourceModel()->data(acceptanceDateIndex).toString();

    // Фильтр по типу выплаты
    if (!m_paymentTypeFilter.isEmpty() && m_paymentTypeFilter != "Все") {
        if (type != m_paymentTypeFilter) return false;
    }

    // Фильтр по статусу (для моих заявок)
    if (!m_statusFilter.isEmpty() && m_statusFilter != "Все") {
        if (status != m_statusFilter) return false;
    }

    // Фильтр по дате подачи
    if (m_useDateFilter && !dateStr.isEmpty()) {
        QDate date = QDate::fromString(dateStr, "dd.MM.yyyy");
        if (date != m_dateFilter) return false;
    }

    // Фильтр по дате принятия
    if (m_useAcceptanceDateFilter && !acceptanceDateStr.isEmpty()) {
        QDate date = QDate::fromString(acceptanceDateStr, "dd.MM.yyyy");
        if (date != m_acceptanceDateFilter) return false;
    }

    return true;
}
