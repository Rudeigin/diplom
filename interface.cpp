#include "interface.h"

#include "pageDefines.h"

#include <QDebug>
#include <QDir>
#include <QFontMetrics>
#include <QPainter>
#include <QPdfWriter>
#include <poppler/qt5/poppler-qt5.h>
Interface::Interface(QObject* parent): QObject(parent) {
    qRegisterMetaType<Form*>();
    qRegisterMetaType<Question*>();
    qRegisterMetaType<Answer*>();
    qRegisterMetaType<ListModel*>();

    m_model = new ListModel(this);
    loadForms();
}

Interface::~Interface() {
    dumpForms();
}

void Interface::addForm(QString title) {
    Form* frm = new Form(this);
    frm->setTitle(title);
    m_model->appendRow(frm);

    emit modelChanged(m_model);
}

void Interface::addQst(Form *frm) {
    frm->addQuestion();
}

void Interface::addAnsw(Question *qst) {
    qst->addAnswer();
}

/*
 * Памятка, почему я не разделила заполнение координат и создание pdf:
 * координаты необходимы только при уже созданном когда-то pdf,
 * поэтому пересчитывать каждый раз приредактировании анкеты не имеет смысла,
 * проще посчитать при создании нового pdf-файла
 */

void Interface::createPdf(Form* form) {
    const QString fileName(QDir::homePath() + QDir::separator() + form->title() + ".pdf");

    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));

    QPainter painter(&pdfWriter);
    QFontMetrics metric(painter.font());

    // Размеры области для рисования в пикселях
    QRectF pageSize = pdfWriter.pageLayout().paintRectPixels(pdfWriter.resolution());

    int usedPageHeight = 0;
    foreach(ListItem* q, form->questions()->items()) {
        Question* qst = qobject_cast<Question*>(q);
        // рассчитываем границы вопроса
        QRect qstRect = metric.boundingRect(pageSize.toRect(), Qt::TextWordWrap, qst->text());
        if(usedPageHeight + qstRect.height() > pageSize.height()) {
            pdfWriter.newPage();
            usedPageHeight = 0;
        }
        painter.drawText(
            pageSize.x(),
            pageSize.y() + usedPageHeight,
            pageSize.width(),
            pageSize.height() - usedPageHeight,
            Qt::TextWordWrap,
            tr("%1. %2").arg(qst->number()).arg(qst->text()));

        usedPageHeight += qstRect.height() + LINE_SPACING;

        foreach(ListItem* a, qst->answers()->items()) {
            Answer* answ = qobject_cast<Answer*>(a);
            // рассчитываем границы ответа
            QRect answRect = metric.boundingRect(
                pageSize.toRect(), Qt::TextWordWrap, answ->text());
            // рисуем вопрос на листе
            painter.drawText(
                pageSize.x(),
                pageSize.y() + usedPageHeight,
                pageSize.width(),
                pageSize.height() - usedPageHeight,
                Qt::TextWordWrap,
                tr("%1. %2").arg(answ->number()).arg(answ->text()));
            usedPageHeight += answRect.height() + LINE_SPACING;
        }
        usedPageHeight += (metric.lineSpacing() - LINE_SPACING);
    }
    painter.end();

    Poppler::Document* document = Poppler::Document::load(fileName);
    if (!document || document->isLocked()) {
        // ... error message ....
        delete document;
        return;
    }
    QImage image = document->page(0)->renderToImage(pdfWriter.resolution(), pdfWriter.resolution(),
                                                    0, 0, pageSize.width(), pageSize.height());
    if (image.isNull()) {
        // ... error message ...
        return;
    }
    image.save("aaa2.jpeg");
    delete document;
}

void Interface::dumpForms() {

}

void Interface::loadForms() {

}

ListModel* Interface::model() {
    return m_model;
}

Form* Interface::getForm(int index) {
    return qobject_cast<Form*>(m_model->row(index));
}

Question *Interface::getQst(Form *form, int index)
{
    return qobject_cast<Question *>(form->questions()->row(index));
}
