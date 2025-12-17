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

PdfDocumentGenerator::PdfDocumentGenerator(QObject *parent)
    : QObject(parent)
{
}

QByteArray PdfDocumentGenerator::generateCertificate(const ApplicationInfo &appInfo)
{
    QTextDocument document;
    setupDocument(document, appInfo);

    QBuffer buffer;
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName("temp.pdf");
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(20, 20, 20, 20), QPageLayout::Millimeter);

    // Рендерим документ в PDF
    document.print(&printer);

    // Читаем из файла
    QFile file("temp.pdf");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.remove(); // Удаляем временный файл
        return data;
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

    QTextCursor cursor(&document);

    // Заголовок
    QTextCharFormat titleFormat;
    titleFormat.setFontPointSize(16);
    titleFormat.setFontWeight(QFont::Bold);
    titleFormat.setFontCapitalization(QFont::AllUppercase);
    titleFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);

    QTextBlockFormat centerFormat;
    centerFormat.setAlignment(Qt::AlignCenter);

    cursor.insertBlock(centerFormat);
    cursor.insertText("СПРАВКА", titleFormat);

    // Подзаголовок
    cursor.insertBlock();
    QTextCharFormat subtitleFormat;
    subtitleFormat.setFontPointSize(14);
    cursor.insertText("о принятии заявки на социальную выплату", subtitleFormat);

    // Информация о документе
    cursor.insertBlock();
    QTextBlockFormat rightFormat;
    rightFormat.setAlignment(Qt::AlignRight);
    cursor.insertBlock(rightFormat);

    QTextCharFormat infoFormat;
    infoFormat.setFontPointSize(10);
    cursor.insertText(QString("Дата формирования: %1\n").arg(currentDateTime.toString("dd.MM.yyyy HH:mm:ss")), infoFormat);
    cursor.insertText(QString("Номер документа: СП-%1-%2\n").arg(appInfo.applicationId).arg(currentDateTime.toString("yyyyMMdd")), infoFormat);

    // Таблица с данными
    cursor.insertBlock();

    QTextTableFormat tableFormat;
    tableFormat.setHeaderRowCount(1);
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tableFormat.setBorder(1);
    tableFormat.setCellSpacing(0);
    tableFormat.setCellPadding(5);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));

    QTextTable *table = cursor.insertTable(12, 2, tableFormat);

    // Заголовки таблицы
    QTextCharFormat headerFormat;
    headerFormat.setFontWeight(QFont::Bold);
    headerFormat.setBackground(QColor(240, 240, 240));

    table->cellAt(0, 0).firstCursorPosition().insertText("Поле", headerFormat);
    table->cellAt(0, 1).firstCursorPosition().insertText("Значение", headerFormat);

    // Данные
    QStringList headers = {
        "Номер заявки",
        "Заявитель",
        "Тип социальной выплаты",
        "Дата подачи заявки",
        "Запрашиваемая сумма",
        "Утвержденная сумма",
        "Дата принятия решения",
        "Ответственный сотрудник",
        "Должность",
        "Филиал",
        "ИНН заявителя",
        "Телефон заявителя"
    };

    QStringList values = {
        QString::number(appInfo.applicationId),
        appInfo.applicantName,
        appInfo.paymentType,
        appInfo.creationDate.toString("dd.MM.yyyy HH:mm"),
        QString::number(appInfo.requestedAmount, 'f', 2) + " руб.",
        QString::number(appInfo.approvedAmount, 'f', 2) + " руб.",
        currentDateTime.toString("dd.MM.yyyy"),
        appInfo.employeeName,
        appInfo.employeePosition,
        appInfo.branchName,
        appInfo.applicantInn,
        appInfo.applicantPhone
    };

    for (int i = 0; i < headers.size(); ++i) {
        table->cellAt(i + 1, 0).firstCursorPosition().insertText(headers[i]);
        table->cellAt(i + 1, 1).firstCursorPosition().insertText(values[i]);
    }

    // Текст справки
    cursor.movePosition(QTextCursor::End);
    cursor.insertBlock();

    QTextCharFormat textFormat;
    textFormat.setFontPointSize(12);
    cursor.setCharFormat(textFormat);

    cursor.insertText("\nНастоящая справка подтверждает, что заявка на получение социальной выплаты была рассмотрена и принята к исполнению.\n");
    cursor.insertText("Справка является официальным документом и может быть использована для получения указанной социальной выплаты в соответствии с установленным порядком.\n");

    // Подпись
    cursor.insertBlock();
    QTextBlockFormat signatureFormat;
    signatureFormat.setAlignment(Qt::AlignRight);
    cursor.insertBlock(signatureFormat);

    cursor.insertText("\n_________________________\n");
    cursor.insertText("(Подпись сотрудника)\n");

    cursor.insertBlock(signatureFormat);
    cursor.insertText("\nМ.П.\n");

    // Футер
    cursor.insertBlock();
    QTextBlockFormat footerFormat;
    footerFormat.setAlignment(Qt::AlignCenter);
    cursor.insertBlock(footerFormat);

    QTextCharFormat footerCharFormat;
    footerCharFormat.setFontPointSize(8);
    footerCharFormat.setFontItalic(true);
    cursor.setCharFormat(footerCharFormat);

    cursor.insertText("\n\nДокумент сформирован автоматически системой\n");
    cursor.insertText("\"Интерактивный помощник для составления заявлений на получение социальных выплат\"\n");
    cursor.insertText(QString("Дата и время формирования: %1").arg(currentDateTime.toString("dd.MM.yyyy HH:mm:ss")));
}
