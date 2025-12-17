#include "acceptedapplicationsmodel.h"

AcceptedApplicationsModel::AcceptedApplicationsModel(int applicantId, QObject *parent)
    : QAbstractTableModel(parent), m_applicantId(applicantId)
{
    refreshData();
}

int AcceptedApplicationsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_applications.size();
}

int AcceptedApplicationsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4; // ID, Тип выплаты, Дата принятия, Сумма (документ скрыт)
}

QVariant AcceptedApplicationsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_applications.size())
        return QVariant();

    const AcceptedApplicationData &app = m_applications.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return app.applicationId;
        case 1: return app.paymentType;
        case 2: return app.acceptanceDate.toString("dd.MM.yyyy");
        case 3: return QString::number(app.amount, 'f', 2);
        }
    } else if (role == Qt::UserRole) {
        // Возвращаем данные для разных колонок
        switch (index.column()) {
        case 4: return app.certificateData; // Документ
        case 5: return app.certificateFileName; // Имя файла
        default: return app.applicationId;
        }
    }

    return QVariant();
}

QVariant AcceptedApplicationsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return "ID";
        case 1: return "Тип выплаты";
        case 2: return "Дата принятия";
        case 3: return "Сумма";
        }
    }
    return QVariant();
}

AcceptedApplicationData AcceptedApplicationsModel::getApplicationData(int row) const
{
    if (row >= 0 && row < m_applications.size())
        return m_applications.at(row);
    return AcceptedApplicationData();
}

void AcceptedApplicationsModel::refreshData()
{
    beginResetModel();
    m_applications.clear();

    QSqlQuery query;
    query.prepare(
        "SELECT a.application_id, pt.type_name, a.acceptance_date, "
        "a.amount, c.document, c.file_name "
        "FROM application a "
        "JOIN payment_type pt ON a.payment_type_id = pt.payment_type_id "
        "LEFT JOIN certificate c ON a.application_id = c.application_id "
        "WHERE a.applicant_id = :applicant_id "
        "AND a.status_id = (SELECT status_id FROM status WHERE status_name = 'Принята') "
        "AND a.acceptance_date IS NOT NULL "
        "ORDER BY a.acceptance_date DESC"
        );
    query.bindValue(":applicant_id", m_applicantId);

    if (query.exec()) {
        while (query.next()) {
            AcceptedApplicationData app;
            app.applicationId = query.value(0).toInt();
            app.paymentType = query.value(1).toString();
            app.acceptanceDate = query.value(2).toDate();
            app.amount = query.value(3).toDouble();
            app.certificateData = query.value(4).toByteArray();
            app.certificateFileName = query.value(5).toString();
            m_applications.append(app);
        }
    }

    endResetModel();
}
