#include "empapplicationsmodel.h"

EmpApplicationsModel::EmpApplicationsModel(int employeeId, QObject *parent)
    : QAbstractTableModel(parent)
    , m_employeeId(employeeId)
{
    refreshData();
}

int EmpApplicationsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_applications.size();
}

int EmpApplicationsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5; // ID, Заявитель, Тип выплаты, Дата создания, Сумма
}

QVariant EmpApplicationsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_applications.size())
        return QVariant();

    const EmpApplicationData &app = m_applications.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return app.applicationId;
        case 1: return app.applicantName;
        case 2: return app.paymentType;
        case 3: return app.creationDate.toString("dd.MM.yyyy HH:mm");
        case 4: return QString::number(app.amount, 'f', 2) + " руб.";
        }
    } else if (role == Qt::UserRole) {
        return app.applicationId;
    }

    return QVariant();
}

QVariant EmpApplicationsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0: return "ID заявки";
        case 1: return "Заявитель";
        case 2: return "Тип выплаты";
        case 3: return "Дата создания";
        case 4: return "Сумма";
        }
    }

    return QVariant();
}

void EmpApplicationsModel::refreshData()
{
    beginResetModel();

    m_applications.clear();

    QSqlQuery query;
    query.prepare(
        "SELECT "
        "    a.application_id, "
        "    CONCAT(app.surname, ' ', app.name, ' ', COALESCE(app.patronymic, '')) as applicant_name, "
        "    tosp.type_name, "
        "    a.date_of_creation, "
        "    a.amount, "
        "    ast.status "
        "FROM application a "
        "JOIN applicant app ON a.applicant_id = app.applicant_id "
        "JOIN type_of_social_payment tosp ON a.type_of_social_payment_id = tosp.type_of_social_payment_id "
        "JOIN application_status ast ON a.application_status_id = ast.application_status_id "
        "WHERE a.employee_id = :employee_id "
        "  AND a.application_status_id = 1 " // Только на рассмотрении
        "ORDER BY a.date_of_creation DESC"
        );
    query.bindValue(":employee_id", m_employeeId);

    if (query.exec()) {
        while (query.next()) {
            EmpApplicationData app;
            app.applicationId = query.value(0).toInt();
            app.applicantName = query.value(1).toString();
            app.paymentType = query.value(2).toString();
            app.creationDate = query.value(3).toDateTime();
            app.amount = query.value(4).toDouble();
            app.status = query.value(5).toString();

            // Формируем дополнительную информацию для отображения
            app.additionalInfo = QString(
                                     "<h2>Детали заявки #%1</h2>"
                                     "<table border='1' cellpadding='5'>"
                                     "<tr><td><b>ID заявки:</b></td><td>%1</td></tr>"
                                     "<tr><td><b>Заявитель:</b></td><td>%2</td></tr>"
                                     "<tr><td><b>Тип выплаты:</b></td><td>%3</td></tr>"
                                     "<tr><td><b>Дата создания:</b></td><td>%4</td></tr>"
                                     "<tr><td><b>Сумма:</b></td><td>%5 руб.</td></tr>"
                                     "<tr><td><b>Статус:</b></td><td>%6</td></tr>"
                                     "</table>"
                                     ).arg(app.applicationId)
                                     .arg(app.applicantName)
                                     .arg(app.paymentType)
                                     .arg(app.creationDate.toString("dd.MM.yyyy HH:mm"))
                                     .arg(QString::number(app.amount, 'f', 2))
                                     .arg(app.status);

            m_applications.append(app);
        }
    } else {
        qDebug() << "Ошибка загрузки заявок:" << query.lastError().text();
    }

    endResetModel();
}

EmpApplicationData EmpApplicationsModel::getApplicationData(int row) const
{
    if (row >= 0 && row < m_applications.size()) {
        return m_applications.at(row);
    }
    return EmpApplicationData();
}
