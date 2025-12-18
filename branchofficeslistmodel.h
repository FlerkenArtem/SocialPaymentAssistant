#ifndef BRANCHOFFICESLISTMODEL_H
#define BRANCHOFFICESLISTMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QString>

// Структура данных для филиала
struct BranchOffice {
    int id;
    QString name;
    QString city;
    QString address;
    QString phone;
    QString email;
};

// Модель данных для таблицы филиалов
class BranchOfficesListModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit BranchOfficesListModel(QObject *parent = nullptr);
    ~BranchOfficesListModel();

    // Методы QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Обновление данных из БД
    void updateData();

private:
    QList<BranchOffice> branchOffices;
};

#endif // BRANCHOFFICESLISTMODEL_H
