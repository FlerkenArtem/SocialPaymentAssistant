#include "myapplicationsmodel.h"

MyApplicationsModel::MyApplicationsModel(int applicantId, QObject *parent)
    : QAbstractTableModel(parent), m_applicantId(applicantId)
{
    refreshData();
}

int MyApplicationsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_applications.size();
}

int MyApplicationsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 7; // ID, Тип выплаты, Статус, Дата подачи, Сумма, Сотрудник, Дата рассмотрения
}

QVariant MyApplicationsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_applications.size())
        return QVariant();

    const ApplicationData &app = m_applications.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return app.applicationId;
        case 1: return app.paymentType;
        case 2: return app.status;
        case 3: return app.applicationDate.toString("dd.MM.yyyy");
        case 4: return QString::number(app.amount, 'f', 2);
        case 5: return app.employeeName.isEmpty() ? "Не назначен" : app.employeeName;
        case 6: return app.considerationDate.isValid() ? app.considerationDate.toString("dd.MM.yyyy") : "Не рассмотрена";
        }
    } else if (role == Qt::UserRole) {
        return app.applicationId; // Для получения ID заявки
    }

    return QVariant();
}

QVariant MyApplicationsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return "ID";
        case 1: return "Тип выплаты";
        case 2: return "Статус";
        case 3: return "Дата подачи";
        case 4: return "Сумма";
        case 5: return "Ответственный";
        case 6: return "Дата рассмотрения";
        }
    }
    return QVariant();
}

ApplicationData MyApplicationsModel::getApplicationData(int row) const
{
    if (row >= 0 && row < m_applications.size())
        return m_applications.at(row);
    return ApplicationData();
}

void MyApplicationsModel::refreshData()
{
    beginResetModel();
    m_applications.clear();

    QSqlQuery query;
    query.prepare(
        "SELECT a.application_id, pt.type_name, s.status_name, "
        "a.application_date, a.amount, "
        "COALESCE(e.surname || ' ' || LEFT(e.name, 1) || '. ' || LEFT(e.patronymic, 1) || '.', ''), "
        "a.consideration_date "
        "FROM application a "
        "JOIN payment_type pt ON a.payment_type_id = pt.payment_type_id "
        "JOIN status s ON a.status_id = s.status_id "
        "LEFT JOIN employee e ON a.employee_id = e.employee_id "
        "WHERE a.applicant_id = :applicant_id "
        "ORDER BY a.application_date DESC"
        );
    query.bindValue(":applicant_id", m_applicantId);

    if (query.exec()) {
        while (query.next()) {
            ApplicationData app;
            app.applicationId = query.value(0).toInt();
            app.paymentType = query.value(1).toString();
            app.status = query.value(2).toString();
            app.applicationDate = query.value(3).toDate();
            app.amount = query.value(4).toDouble();
            app.employeeName = query.value(5).toString();
            app.considerationDate = query.value(6).toDate();
            m_applications.append(app);
        }
    }

    endResetModel();
}
