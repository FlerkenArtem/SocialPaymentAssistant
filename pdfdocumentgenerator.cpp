#include "pdfdocumentgenerator.h"
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QTextTableFormat>
#include <QTextCharFormat>
#include <QTextBlockFormat>
#include <QPrinter>
#include <QBuffer>
#include <QFile>
#include <QDir>

PdfDocumentGenerator::PdfDocumentGenerator(QObject *parent)
    : QObject(parent)
{
}

QByteArray PdfDocumentGenerator::generateCertificate(const ApplicationInfo &appInfo)
{
    QTextDocument document;
    setupDocument(document, appInfo);

    // Создаем уникальное имя временного файла
    QString tempFileName = QDir::tempPath() + QString("/certificate_temp_%1_%2.pdf")
                                                  .arg(appInfo.applicationId)
                                                  .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));

    qDebug() << "Генерация PDF во временный файл:" << tempFileName;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(tempFileName);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(20, 20, 20, 20), QPageLayout::Millimeter);
    printer.setCreator("Интерактивный помощник для социальных выплат");
    printer.setDocName(QString("Справка №%1").arg(appInfo.applicationId));

    try {
        // Рендерим документ в PDF
        document.print(&printer);
    } catch (const std::exception &e) {
        qWarning() << "Ошибка при печати в PDF:" << e.what();
        return QByteArray();
    }

    // Читаем файл
    QFile file(tempFileName);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        // Удаляем временный файл
        if (!QFile::remove(tempFileName)) {
            qWarning() << "Не удалось удалить временный файл:" << tempFileName;
        }

        qDebug() << "PDF сгенерирован, размер:" << data.size() << "байт";
        return data;
    } else {
        qWarning() << "Не удалось открыть временный файл:" << tempFileName;
    }

    return QByteArray();
}

bool PdfDocumentGenerator::saveToFile(const QString &filePath, const ApplicationInfo &appInfo)
{
    QTextDocument document;
    setupDocument(document, appInfo);

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(20, 20, 20, 20), QPageLayout::Millimeter);
    printer.setCreator("Интерактивный помощник для социальных выплат");
    printer.setDocName(QString("Справка №%1").arg(appInfo.applicationId));

    document.print(&printer);
    return QFile::exists(filePath);
}

void PdfDocumentGenerator::setupDocument(QTextDocument &document, const ApplicationInfo &appInfo)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();

    QString htmlContent = QString(
                              "<html>"
                              "<head>"
                              "<style>"
                              "body { font-family: Arial, sans-serif; margin: 40px; }"
                              "h1 { text-align: center; font-size: 20px; font-weight: bold; text-decoration: underline; }"
                              "h2 { text-align: center; font-size: 16px; font-weight: bold; }"
                              "table { width: 100%; border-collapse: collapse; margin: 20px 0; }"
                              "th { background-color: #f2f2f2; font-weight: bold; padding: 10px; border: 1px solid #ddd; text-align: left; }"
                              "td { padding: 10px; border: 1px solid #ddd; }"
                              ".signature { text-align: right; margin-top: 50px; }"
                              ".footer { font-size: 10px; text-align: center; font-style: italic; margin-top: 30px; }"
                              "</style>"
                              "</head>"
                              "<body>"
                              "<h1>СПРАВКА</h1>"
                              "<h2>о принятии заявки на социальную выплату</h2>"

                              "<p style='text-align: right; font-size: 10px;'>"
                              "Дата формирования: %1<br>"
                              "Номер документа: СП-%2-%3"
                              "</p>"

                              "<table>"
                              "<tr><th>Поле</th><th>Значение</th></tr>"
                              "<tr><td>Номер заявки</td><td>%2</td></tr>"
                              "<tr><td>Заявитель</td><td>%4</td></tr>"
                              "<tr><td>Тип социальной выплаты</td><td>%5</td></tr>"
                              "<tr><td>Дата подачи заявки</td><td>%6</td></tr>"
                              "<tr><td>Запрашиваемая сумма</td><td>%7 руб.</td></tr>"
                              "<tr><td>Утвержденная сумма</td><td>%8 руб.</td></tr>"
                              "<tr><td>Дата принятия решения</td><td>%9</td></tr>"
                              "<tr><td>Ответственный сотрудник</td><td>%10</td></tr>"
                              "<tr><td>Должность</td><td>%11</td></tr>"
                              "<tr><td>ИНН заявителя</td><td>%12</td></tr>"
                              "<tr><td>Телефон заявителя</td><td>%13</td></tr>"
                              "</table>"

                              "<p>Настоящая справка подтверждает, что заявка на получение социальной выплаты была рассмотрена и принята к исполнению.</p>"
                              "<p>Справка является официальным документом и может быть использована для получения указанной социальной выплаты в соответствии с установленным порядком.</p>"

                              "<div class='signature'>"
                              "_________________________<br>"
                              "(Подпись сотрудника)<br><br>"
                              "М.П."
                              "</div>"

                              "<div class='footer'>"
                              "Документ сформирован автоматически системой<br>"
                              "\"Интерактивный помощник для составления заявлений на получение социальных выплат\"<br>"
                              "Дата и время формирования: %1"
                              "</div>"
                              "</body>"
                              "</html>"
                              )
                              .arg(currentDateTime.toString("dd.MM.yyyy HH:mm:ss"))
                              .arg(appInfo.applicationId)
                              .arg(currentDateTime.toString("yyyyMMdd"))
                              .arg(appInfo.applicantName)
                              .arg(appInfo.paymentType)
                              .arg(appInfo.creationDate.toString("dd.MM.yyyy HH:mm"))
                              .arg(QString::number(appInfo.requestedAmount, 'f', 2))
                              .arg(QString::number(appInfo.approvedAmount, 'f', 2))
                              .arg(currentDateTime.toString("dd.MM.yyyy"))
                              .arg(appInfo.employeeName)
                              .arg(appInfo.employeePosition)
                              .arg(appInfo.applicantInn)
                              .arg(appInfo.applicantPhone);

    document.setHtml(htmlContent);
}
