#ifndef EMPAPPLICATIONSMODEL_H
#define EMPAPPLICATIONSMODEL_H

#include <QAbstractTableModel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

struct EmpApplicationData {
    int applicationId;
    QString applicantName;
    QString paymentType;
    QDateTime creationDate;
    double amount;
    QString status;
    QString additionalInfo;
};

class EmpApplicationsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit EmpApplicationsModel(int employeeId, QObject *parent = nullptr);

    // QAbstractTableModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Обновление данных
    void refreshData();

    // Получение данных заявки по строке
    EmpApplicationData getApplicationData(int row) const;

private:
    int m_employeeId;
    QVector<EmpApplicationData> m_applications;
};

#endif // EMPAPPLICATIONSMODEL_H
