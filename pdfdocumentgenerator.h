#ifndef PDFDOCUMENTGENERATOR_H
#define PDFDOCUMENTGENERATOR_H

#include <QObject>
#include <QByteArray>
#include <QDateTime>
#include <QTextDocument>

class PdfDocumentGenerator : public QObject
{
    Q_OBJECT

public:
    struct ApplicationInfo {
        int applicationId;
        QString applicantName;
        QString paymentType;
        QDateTime creationDate;
        double requestedAmount;
        double approvedAmount;
        QString employeeName;
        QString employeePosition;
        QString branchName;
        QString applicantAddress;
        QString applicantInn;
        QString applicantPhone;
    };

    explicit PdfDocumentGenerator(QObject *parent = nullptr);

    QByteArray generateCertificate(const ApplicationInfo &appInfo);
    bool saveToFile(const QString &filePath, const ApplicationInfo &appInfo);

private:
    void setupDocument(QTextDocument &document, const ApplicationInfo &appInfo);
};

#endif // PDFDOCUMENTGENERATOR_H
