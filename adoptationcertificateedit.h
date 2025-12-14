#ifndef ADOPTATIONCERTIFICATEEDIT_H
#define ADOPTATIONCERTIFICATEEDIT_H

#include <QWidget>

namespace Ui {
class AdoptationCertificateEdit;
}

class AdoptationCertificateEdit : public QWidget
{
    Q_OBJECT

public:
    explicit AdoptationCertificateEdit(QWidget *parent = nullptr);
    ~AdoptationCertificateEdit();

private:
    Ui::AdoptationCertificateEdit *ui;
};

#endif // ADOPTATIONCERTIFICATEEDIT_H
