#ifndef MYAPPLICATIONSMODEL_H
#define MYAPPLICATIONSMODEL_H

#include <QAbstractTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>

struct ApplicationData {
    int applicationId;
    QString paymentType;
    QString status;
    QDate applicationDate;
    double amount;
    QString employeeName;
    QDate considerationDate;
};

class MyApplicationsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit MyApplicationsModel(int applicantId, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    ApplicationData getApplicationData(int row) const;
    void refreshData();

private:
    int m_applicantId;
    QList<ApplicationData> m_applications;
};

#endif // MYAPPLICATIONSMODEL_H
