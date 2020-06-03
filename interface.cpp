#include "interface.h"

#include "pageDefines.h"

#include <poppler/qt5/poppler-qt5.h>

#include <QDebug>
#include <QDir>
#include <QFontMetrics>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QPainter>
#include <QPdfWriter>
#include <QtConcurrent/QtConcurrent>

Interface::Interface(QObject* parent): QObject(parent) {
    // для ListModel
    qRegisterMetaType<Answer>();
    qRegisterMetaType<Question>();
    qRegisterMetaType<Form>();
    // для графики
    qRegisterMetaType<Answer*>();
    qRegisterMetaType<Question*>();
    qRegisterMetaType<Form*>();
    qRegisterMetaType<ListModel*>();

    _model = new ListModel(this);
    _formConfigName = "forms.json";
    _dataDirPath = QDir::homePath() + QDir::separator() + "qstmanager_data" + QDir::separator();
    loadForms();
}

Interface::~Interface() {
    dumpForms();
}

void Interface::addForm(QString title) {
    Form* frm = new Form(this);
    frm->setTitle(title);
    _model->appendRow(frm);

    emit modelChanged(_model);
}

void Interface::addQst(Form* frm) {
    frm->addQuestion();
}

void Interface::addAnsw(Question* qst) {
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
            QRect answRect = metric.boundingRect(pageSize.toRect(), Qt::TextWordWrap, answ->text());
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
    if(!document || document->isLocked()) {
        // TODO ... error message ....
        delete document;
        return;
    }
    QImage image = document->page(0)->renderToImage(
        pdfWriter.resolution(), pdfWriter.resolution(), 0, 0, pageSize.width(), pageSize.height());
    if(image.isNull()) {
        // TODO ... error message ...
        return;
    }
    image.save("aaa2.jpeg");
    delete document;
}

void Interface::processForms(Form* form, QString pathToPics) {
    _future = QtConcurrent::run(this, &Interface::processPics, form, pathToPics);
}

void Interface::processPics(Form* form, QString pathToPics) {
    Q_UNUSED(form)

    QDir dirInf(pathToPics);
    emit formProcessingTotalCount(dirInf.entryList(QDir::Files).count());

    int progress = 0;
    foreach(QString fileName, dirInf.entryList(QDir::Files)) {
        // TODO тут часть во имя opencv
        system("pause 10");
        emit formProcessingProgress(progress++);
    }
    emit formProcessingFinished("");
}

void Interface::dumpForms() {
    if(!QDir().mkpath(QDir::cleanPath(_dataDirPath))) {
        return;
    }

    QFile config(QDir::cleanPath(_dataDirPath + _formConfigName));
    if(config.open(QIODevice::WriteOnly)) {
        QVariantMap data;
        data.insert("model", _model->toMap());
        QJsonDocument jsdoc(QJsonObject::fromVariantMap(data));
        QByteArray bytearray = jsdoc.toJson(QJsonDocument::Indented);

        config.write(bytearray);
        config.close();

        qDebug() << "Выгружено " << _model->count() << " анкет";
    }
}

void Interface::loadForms() {
    QFile config(QDir::cleanPath(_dataDirPath + _formConfigName));

    if(config.open(QIODevice::ReadOnly)) {
        QByteArray ba = config.readAll();
        config.close();

        QJsonParseError err;
        QJsonDocument jsdoc = jsdoc.fromJson(ba, &err);
        if(err.error == QJsonParseError::NoError) {
            QVariantMap configMap = jsdoc.object().toVariantMap();
            _model->initFromMap(configMap.value("model").toMap());
            qDebug() << "Загружено " << _model->count() << " анкет";
            //            qDebug() << configMap;
        }
    }
}

ListModel* Interface::model() {
    return _model;
}

Form* Interface::getForm(int index) {
    return qobject_cast<Form*>(_model->row(index));
}

Question* Interface::getQst(Form* form, int index) {
    return qobject_cast<Question*>(form->questions()->row(index));
}
