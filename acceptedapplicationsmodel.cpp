#include "acceptedapplicationsmodel.h"
#include "qsqlerror.h"

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
        switch (index.column()) {
        case 4: return app.certificateData;
        case 5: return app.certificateFileName; // Пустая строка
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
        "SELECT a.application_id, tosp.type_name, "
        "c.date_and_time_of_creation, " // acceptance_date заменен на date_and_time_of_creation из certificate
        "a.amount, c.document "
        "FROM application a "
        "JOIN type_of_social_payment tosp ON a.type_of_social_payment_id = tosp.type_of_social_payment_id "
        "JOIN certificate c ON a.application_id = c.application_id "
        "WHERE a.applicant_id = :applicant_id "
        "AND a.application_status_id = (SELECT application_status_id FROM application_status WHERE status = 'Принята') "
        "ORDER BY c.date_and_time_of_creation DESC"
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
            app.certificateFileName = ""; // В ЛР5 нет поля file_name в certificate
            m_applications.append(app);
        }
    } else {
        qDebug() << "Ошибка AcceptedApplicationsModel:" << query.lastError().text();
        qDebug() << "Запрос:" << query.lastQuery();
    }

    endResetModel();
}
