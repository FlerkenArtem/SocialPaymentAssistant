#include "branchofficeslistmodel.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

BranchOfficesListModel::BranchOfficesListModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    updateData();
}

BranchOfficesListModel::~BranchOfficesListModel()
{
}

int BranchOfficesListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return branchOffices.size();
}

int BranchOfficesListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 6; // ID, Название, Город, Адрес, Телефон, Email
}

QVariant BranchOfficesListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= branchOffices.size())
        return QVariant();

    const BranchOffice &office = branchOffices.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return office.id;
        case 1: return office.name;
        case 2: return office.city;
        case 3: return office.address;
        case 4: return office.phone;
        case 5: return office.email;
        }
    }
    else if (role == Qt::TextAlignmentRole) {
        if (index.column() == 0) // ID выровнять по центру
            return Qt::AlignCenter;
    }
    else if (role == Qt::ToolTipRole) {
        return QString("Филиал: %1\nГород: %2\nАдрес: %3\nТелефон: %4\nEmail: %5")
            .arg(office.name, office.city, office.address, office.phone, office.email);
    }

    return QVariant();
}

QVariant BranchOfficesListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0: return "ID";
        case 1: return "Название филиала";
        case 2: return "Город";
        case 3: return "Адрес";
        case 4: return "Телефон";
        case 5: return "Email";
        }
    }
    return QVariant();
}

void BranchOfficesListModel::updateData()
{
    beginResetModel();
    branchOffices.clear();

    // SQL-запрос для получения данных о филиалах
    QSqlQuery query;
    query.prepare(
        "SELECT "
        "   bo.branch_office_id, "
        "   bo.branch_name, "
        "   c.name_city, "
        "   CONCAT(s.name_street, ', д. ', h.name_house, "
        "       CASE WHEN f.name_flat IS NOT NULL THEN CONCAT(', кв. ', f.name_flat) ELSE '' END) AS address, "
        "   bo.phone_number, "
        "   bo.email "
        "FROM branch_office bo "
        "JOIN address a ON bo.address_id = a.address_id "
        "JOIN city c ON a.city_id = c.city_id "
        "JOIN street s ON a.street_id = s.street_id "
        "JOIN house h ON a.house_id = h.house_id "
        "LEFT JOIN flat f ON a.flat_id = f.flat_id "
        "ORDER BY c.name_city, bo.branch_name"
        );

    if (query.exec()) {
        while (query.next()) {
            BranchOffice office;
            office.id = query.value(0).toInt();
            office.name = query.value(1).toString();
            office.city = query.value(2).toString();
            office.address = query.value(3).toString();
            office.phone = query.value(4).toString();
            office.email = query.value(5).toString();

            branchOffices.append(office);
        }
    } else {
        qDebug() << "Ошибка загрузки филиалов:" << query.lastError().text();
    }

    endResetModel();
}
