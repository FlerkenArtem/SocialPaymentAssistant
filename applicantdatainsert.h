#ifndef APPLICANTDATAINSERT_H
#define APPLICANTDATAINSERT_H

#include <QWidget>

namespace Ui {
class ApplicantDataInsert;
}

class ApplicantDataInsert : public QWidget
{
    Q_OBJECT

public:
    explicit ApplicantDataInsert(QWidget *parent = nullptr);
    ~ApplicantDataInsert();

private:
    Ui::ApplicantDataInsert *ui;
};

#endif // APPLICANTDATAINSERT_H
