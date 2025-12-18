#ifndef APPLICANTREG_H
#define APPLICANTREG_H

#include <QWidget>
#include <QSqlQuery>
#include <QMessageBox>
#include <QFileDialog>
#include <QBuffer>
#include <QDate>

namespace Ui {
class ApplicantReg;
}

class ApplicantReg : public QWidget
{
    Q_OBJECT

public:
    explicit ApplicantReg(QWidget *parent = nullptr);
    ~ApplicantReg();

private slots:
    void on_applicantRegOK_clicked();
    void on_noPatronymicCheck_toggled(bool checked);
    void on_noActualAddress_toggled(bool checked);
    void on_noAdoptationCertificate_toggled(bool checked);
    void on_passportScan_clicked();
    void on_snilsAddScan_clicked();
    void on_policyAddScan_clicked();
    void on_adopatationCertificateAddScan_clicked();

private:
    Ui::ApplicantReg *ui;
    QByteArray passportScanData;
    QByteArray snilsScanData;
    QByteArray policyScanData;
    QByteArray adoptionCertificateScanData;

    void loadRegions();
    void loadDistricts();
    void loadCities();
    void loadAreas();
    void loadStreets();
    void loadHouses();
    void loadFlats();
    void loadBanks();
    void loadPassportDepartments();
    void enableActualAddressFields(bool enabled);
    void enableAdoptionCertificateFields(bool enabled);
    QByteArray loadScanFile(const QString &title);
};

#endif // APPLICANTREG_H
