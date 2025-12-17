#ifndef ACCEPTEDAPPLICATIONSMODEL_H
#define ACCEPTEDAPPLICATIONSMODEL_H

#include "qdatetime.h"
#include <QAbstractTableModel>
#include <QSqlQuery>
#include <QByteArray>

struct AcceptedApplicationData {
    int applicationId;
    QString paymentType;
    QDate acceptanceDate;
    double amount;
    QByteArray certificateData; // документ
    QString certificateFileName;
};

class AcceptedApplicationsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit AcceptedApplicationsModel(int applicantId, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    AcceptedApplicationData getApplicationData(int row) const;
    void refreshData();

private:
    int m_applicantId;
    QList<AcceptedApplicationData> m_applications;
};

#endif // ACCEPTEDAPPLICATIONSMODEL_H
