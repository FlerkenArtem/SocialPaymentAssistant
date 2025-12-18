#ifndef APPLICATIONSPROXYMODEL_H
#define APPLICATIONSPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QDate>

class ApplicationsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ApplicationsProxyModel(QObject *parent = nullptr);

    void setPaymentTypeFilter(const QString &type);
    void setStatusFilter(const QString &status);
    void setDateFilter(const QDate &date);
    void setAcceptanceDateFilter(const QDate &date);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    QString m_paymentTypeFilter;
    QString m_statusFilter;
    QDate m_dateFilter;
    QDate m_acceptanceDateFilter;
    bool m_useDateFilter = false;
    bool m_useAcceptanceDateFilter = false;
};

#endif // APPLICATIONSPROXYMODEL_H
