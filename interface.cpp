#include "interface.h"

#include "pageDefines.h"

#include <poppler/qt5/poppler-qt5.h>

#include <QDebug>
#include <QDir>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QPainter>
#include <QPdfWriter>
#include <QScreen>
#include <QSqlQuery>
#include <QtConcurrent/QtConcurrent>
#include <QtGlobal>
#include <QtMath>

#include <unistd.h>

using namespace cv;

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
    qRegisterMetaType<TableSqlModel*>();

    _model = new ListModel(this);
    _formConfigName = "forms.json";
    _dataDirPath = QDir::homePath() + QDir::separator() + "qstmanager_data" + QDir::separator();
    loadForms();

    _a = 2.2766;
    _b = -2.2787; // y = 2.2765543671225856 x + -2.278697114614444
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

void Interface::deleteForm(int index) {
    ListItem* delIt = _model->take(index);
    if(delIt) {
        delIt->deleteLater();
    }
}

void Interface::addQst(Form* frm) {
    frm->addQuestion();
}

void Interface::deleteQst(Form* frm, int index) {
    ListItem* delIt = frm->questions()->take(index);
    if(delIt) {
        delIt->deleteLater();
    }
}

void Interface::addAnsw(Question* qst) {
    qst->addAnswer();
}

void Interface::deleteAnsw(Question* qst, int index) {
    ListItem* delIt = qst->answers()->take(index);
    if(delIt) {
        delIt->deleteLater();
    }
}

double Interface::correl(Form* form, int xQstIndex, int yQstIndex) {
    qDebug() << form->title() << xQstIndex << yQstIndex;
    Question* xQst = qobject_cast<Question*>(form->questions()->row(xQstIndex));
    Question* yQst = qobject_cast<Question*>(form->questions()->row(yQstIndex));

    int xQstNumber = xQst->number();
    int yQstNumber = yQst->number();

    QVector<int> x;
    QVector<int> y;
    QMap<QString, int> xAnswNumbers;
    int i = 1;
    foreach(ListItem* a, xQst->answers()->items()) {
        Answer* ans = qobject_cast<Answer*>(a);
        xAnswNumbers.insert(ans->number(), i);
        i++;
    }
    QMap<QString, int> yAnswNumbers;
    i = 1;
    foreach(ListItem* a, yQst->answers()->items()) {
        Answer* ans = qobject_cast<Answer*>(a);
        yAnswNumbers.insert(ans->number(), i);
        i++;
    }

    // открываем базу данных
    QString dbFileName = _dataDirPath + form->title() + QDir::separator() + "form.db";
    QSqlDatabase temp = QSqlDatabase::addDatabase("QSQLITE");
    temp.setHostName("NameDataBase");
    temp.setDatabaseName(dbFileName);
    if(!temp.open()) {
        qDebug() << "не открылась";
        return 0;
    }
    //    QSqlQuery query1(tr("SELECT 'Вопрос %1' FROM NameTable").arg(xQstNumber), temp);
    QSqlQuery query("SELECT * FROM NameTable", temp);
    int xInd = query.record().indexOf(tr("Вопрос %1").arg(xQstNumber));
    int yInd = query.record().indexOf(tr("Вопрос %1").arg(yQstNumber));
    qDebug() << query.exec();
    while(query.next()) {
        QString answ = query.value(xInd).toString();
        x.push_back(xAnswNumbers.value(answ));
        answ = query.value(yInd).toString();
        y.push_back(yAnswNumbers.value(answ));
    }
    temp.close();

    double cor = 0;
    double xSr = 0;
    double ySr = 0;
    for(int j = 0; j < x.length(); j++) {
        xSr += x.at(j);
        ySr += y.at(j);
    }

    xSr = xSr / double(x.length());
    ySr = ySr / double(y.length());

    double x2Sum = 0;
    double y2Sum = 0;
    double xySum = 0;
    for(int j = 0; j < x.length(); j++) {
        xySum += (x.at(j) - xSr) * (y.at(j) - ySr);
        x2Sum += qPow((x.at(j) - xSr), 2);
        y2Sum += qPow((y.at(j) - ySr), 2);
    }

    cor = xySum / qSqrt(x2Sum * y2Sum);

    return cor;
}

void Interface::regr(Form* form, int xQstIndex, int yQstIndex) {
    Question* xQst = qobject_cast<Question*>(form->questions()->row(xQstIndex));
    Question* yQst = qobject_cast<Question*>(form->questions()->row(yQstIndex));

    int xQstNumber = xQst->number();
    int yQstNumber = yQst->number();

    QVector<int> x;
    QVector<int> y;
    QMap<QString, int> xAnswNumbers;
    int i = 1;
    foreach(ListItem* a, xQst->answers()->items()) {
        Answer* ans = qobject_cast<Answer*>(a);
        xAnswNumbers.insert(ans->number(), i);
        i++;
    }
    QMap<QString, int> yAnswNumbers;
    i = 1;
    foreach(ListItem* a, yQst->answers()->items()) {
        Answer* ans = qobject_cast<Answer*>(a);
        yAnswNumbers.insert(ans->number(), i);
        i++;
    }

    // открываем базу данных
    QString dbFileName = _dataDirPath + form->title() + QDir::separator() + "form.db";
    QSqlDatabase temp = QSqlDatabase::addDatabase("QSQLITE");
    temp.setHostName("NameDataBase");
    temp.setDatabaseName(dbFileName);
    if(!temp.open()) {
        qDebug() << "не открылась";
        return;
    }
    //    QSqlQuery query1(tr("SELECT 'Вопрос %1' FROM NameTable").arg(xQstNumber), temp);
    QSqlQuery query("SELECT * FROM NameTable", temp);
    int xInd = query.record().indexOf(tr("Вопрос %1").arg(xQstNumber));
    int yInd = query.record().indexOf(tr("Вопрос %1").arg(yQstNumber));
    qDebug() << query.exec();
    while(query.next()) {
        QString answ = query.value(xInd).toString();
        x.push_back(xAnswNumbers.value(answ));
        answ = query.value(yInd).toString();
        y.push_back(yAnswNumbers.value(answ));
    }
    temp.close();

    qDebug() << x << y;
}

double Interface::regr(double x) {
    return _a * x + _b;
}

void Interface::createTable(Form* form) {
    QString dbFileName = _dataDirPath + form->title() + QDir::separator() + "form.db";

    if(!QFile(dbFileName).exists()) {
        QFile dbFile(dbFileName);
        if(dbFile.open(QIODevice::WriteOnly)) {
            dbFile.close();
            QSqlDatabase temp = QSqlDatabase::addDatabase("QSQLITE");
            temp.setHostName("NameDataBase");
            temp.setDatabaseName(dbFileName);
            if(!temp.open()) {
                return;
            }
            QSqlQuery query(temp);
            QString queryStr = "CREATE TABLE NameTable ("
                               "N INTEGER PRIMARY KEY AUTOINCREMENT, 'ID Пациента' VARCHAR(255), ";
            foreach(ListItem* it, form->questions()->items()) {
                queryStr += tr("'Вопрос %1' VARCHAR(255), ").arg(it->property("number").toString());
            }
            queryStr.remove(queryStr.length() - 2, 2);
            queryStr += ");";
            if(!query.exec(queryStr)) {
                qDebug() << "DataBase: error of create " << query.lastQuery();
                temp.close();
                return;
            }
            queryStr.clear();
            temp.close();
        }
    }
}

TableSqlModel* Interface::getSqlModel(Form* form) {
    QString dbFileName = _dataDirPath + form->title() + QDir::separator() + "form.db";

    if(!QFile(dbFileName).exists()) {
        QFile dbFile(dbFileName);
        if(dbFile.open(QIODevice::WriteOnly)) {
            dbFile.close();
            QSqlDatabase temp = QSqlDatabase::addDatabase("QSQLITE");
            temp.setHostName("NameDataBase");
            temp.setDatabaseName(dbFileName);
            if(!temp.open()) {
                return nullptr;
            }
            QSqlQuery query(temp);
            QString queryStr = "CREATE TABLE NameTable ("
                               "N INTEGER PRIMARY KEY AUTOINCREMENT, 'ID Пациента' VARCHAR(255), ";
            foreach(ListItem* it, form->questions()->items()) {
                queryStr += tr("'Вопрос %1' VARCHAR(255), ").arg(it->property("number").toString());
            }
            queryStr.remove(queryStr.length() - 2, 2);
            queryStr += ");";
            if(!query.exec(queryStr)) {
                qDebug() << "DataBase: error of create " << query.lastQuery();
                temp.close();
                return nullptr;
            }
            queryStr.clear();
            /*
            queryStr = "INSERT INTO NameTable ('ID Пациента', ";
            foreach(ListItem* it, form->questions()->items()) {
                queryStr += tr("'Вопрос %1', ").arg(it->property("number").toString());
            }
            queryStr.remove(queryStr.length() - 2, 2);
            queryStr += ") VALUES ('1356', ";
            foreach(ListItem* it, form->questions()->items()) { queryStr += "'test', "; }
            queryStr.remove(queryStr.length() - 2, 2);
            queryStr += ");";*/
            /*
            queryStr = "INSERT INTO NameTable ('ID Пациента', 'Вопрос 1', 'Вопрос 2'";
            queryStr += ") VALUES ('1356', 'б', 'б'";
            queryStr += ");";
            if(!query.exec(queryStr)) {
                qDebug() << "DataBase: error of insert " << query.lastQuery();
                temp.close();
                return nullptr;
            }*/
            temp.close();
        } else {
            return nullptr;
        }
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("NameDataBase");
    db.setDatabaseName(dbFileName);

    TableSqlModel* sqlModel;
    if(db.open()) {
        // QObject за собой подчистит
        sqlModel = new TableSqlModel(this);
        sqlModel->setQuery("SELECT * FROM NameTable");
        qDebug() << sqlModel->columnCount();
    } else {
        return nullptr;
    }
    db.close();

    return sqlModel;
}

/*
 * Памятка, почему я не разделила заполнение координат и создание pdf:
 * координаты необходимы только при уже созданном когда-то pdf,
 * поэтому пересчитывать каждый раз приредактировании анкеты не имеет смысла,
 * проще посчитать при создании нового pdf-файла
 */

void Interface::createPdf(Form* form) {
    _future = QtConcurrent::run(this, &Interface::createFormPdf, form);
}

void Interface::createFormPdf(Form* form) {
    QString dirPath = QDir::cleanPath(_dataDirPath + form->title());
    if(!QDir().mkpath(dirPath)) {
        emit pdfCreatingFinished(tr("Ошибка создания директории."));
        return;
    }
    // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    // Poppler нет под Windows 10, fuck... Чекаловой надо под винду... Выкостыливаем
    // как можем и не можем тоже
#ifdef Q_OS_LINUX
    const QString fileNamePdf(dirPath + QDir::separator() + form->title() + ".pdf");
    QPdfWriter pdfWriter(fileNamePdf);
    pdfWriter.setResolution(QGuiApplication::primaryScreen()->logicalDotsPerInch());
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    QPainter painter(&pdfWriter);
    QFontMetrics metric(painter.font());
    // Размеры области для рисования в пикселях
    QRectF pageSize = pdfWriter.pageLayout().paintRectPixels(pdfWriter.resolution());
#elif Q_OS_WINDOWS
    const QString fileNameImage(dirPath + QDir::separator() + form->title() + ".jpeg");
    QPageSize page(QPageSize::A4);
    QImage imgWriter(
        page.sizePixels(QGuiApplication::primaryScreen()->logicalDotsPerInch()),
        QImage::Format_RGB32);
    QPainter imgPainter(&imgWriter);
    QFontMetrics metric(imgPainter.font());

    QPdfWriter tmp("");
    tmp.setResolution(QGuiApplication::primaryScreen()->logicalDotsPerInch());
    tmp.setPageSize(QPageSize(QPageSize::A4));
    // Размеры области для рисования в пикселях
    QRectF pageSize = tmp.pageLayout().paintRectPixels(tmp.resolution());
#endif

    int usedPageHeight = 0;
    // печатаем title
    QRect ttlRect = metric.boundingRect(pageSize.toRect(), Qt::TextWordWrap, form->title());
    int x = pageSize.x() + (pageSize.width() - ttlRect.width()) / 2;
    int y = pageSize.y() + usedPageHeight;
    int width = pageSize.width();
    int height = pageSize.height() - usedPageHeight;
#ifdef Q_OS_LINUX
    painter.drawText(x, y, width, height, Qt::TextWordWrap, tr("%1").arg(form->title()));
#elif Q_OS_WINDOWS
    imgPainter.drawText(x, y, width, height, Qt::TextWordWrap, tr("%1").arg(form->title()));
#endif

    usedPageHeight += ttlRect.height() + LINE_SPACING;
    foreach(ListItem* q, form->questions()->items()) {
        Question* qst = qobject_cast<Question*>(q);
        // рассчитываем границы вопроса
        QString qT = tr("%1. %2").arg(qst->number()).arg(qst->text());
        QRect qstRect = metric.boundingRect(pageSize.toRect(), Qt::TextWordWrap, qT);
        if(usedPageHeight + qstRect.height() > pageSize.height()) {
#ifdef Q_OS_LINUX
            pdfWriter.newPage();
#elif Q_OS_WINDOWS
            // ну да ну да мои полномочия всё
            imgPainter.end();
            qDebug() << "создание завершено, правда, место всё";
            emit pdfCreatingFinished("Создание pdf-файла завершено.");
#endif
            pdfWriter.newPage();
            usedPageHeight = 0;
        }
        x = pageSize.x();
        y = pageSize.y() + usedPageHeight;
        height = pageSize.height() - usedPageHeight;
#ifdef Q_OS_LINUX
        painter.drawText(x, y, width, height, Qt::TextWordWrap, qT);
#elif Q_OS_WINDOWS
        imgPainter.drawText(x, y, width, height, Qt::TextWordWrap, qT);
#endif
        qst->setCoord(
            QRect(pageSize.x(), pageSize.y() + usedPageHeight, qstRect.width(), qstRect.height()));
        usedPageHeight += qstRect.height() + LINE_SPACING;

        foreach(ListItem* a, qst->answers()->items()) {
            Answer* answ = qobject_cast<Answer*>(a);
            // рассчитываем границы ответа
            QString aT = tr("%1. %2").arg(answ->number()).arg(answ->text());
            QRect answRect = metric.boundingRect(pageSize.toRect(), Qt::TextWordWrap, aT);
            // рисуем ответ на листе
            x = pageSize.x();
            y = pageSize.y() + usedPageHeight;
            height = pageSize.height() - usedPageHeight;
#ifdef Q_OS_LINUX
            painter.drawText(x, y, width, height, Qt::TextWordWrap, aT);
#elif Q_OS_WINDOWS
            imgPainter.drawText(x, y, width, height, Qt::TextWordWrap, aT);
#endif
            usedPageHeight += answRect.height() + LINE_SPACING;
            QRect numRect = metric.boundingRect(
                pageSize.toRect(), Qt::TextWordWrap, answ->number() + ". ");
            answ->setCoord(QRect(
                pageSize.x(), pageSize.y() + usedPageHeight, numRect.width(), numRect.height()));
        }
        usedPageHeight += (metric.lineSpacing() - LINE_SPACING);
    }
#ifdef Q_OS_LINUX
    painter.end();
#elif Q_OS_WINDOWS
    imgPainter.end();
    imgWriter.save(fileNameImage);
#endif
    qDebug() << "создание завершено";
    emit pdfCreatingFinished("Создание pdf-файла завершено.");
}

void Interface::processForms(Form* form, QString pathToPics) {
    pathToPics.remove("file://");
    _future = QtConcurrent::run(this, &Interface::processPics, form, pathToPics);
}

void Interface::processPics(Form* form, QString pathToPics) {
    qDebug() << "обработка начата";
    createTable(form);

    QDir dirInf(pathToPics);
    emit formProcessingTotalCount(dirInf.entryList(QDir::Files).count());
    qDebug().noquote() << tr("обнаружено %1 файлов").arg(dirInf.entryList(QDir::Files).count());
    int progress = 0;

#ifdef Q_OS_UNIX
    QString imgFileName = _dataDirPath + QDir::separator() + form->title() + QDir::separator() +
        form->title() + ".pdf";
    Poppler::Document* document = Poppler::Document::load(imgFileName);
    if(!document || document->isLocked()) {
        // TODO ... error message ....
        delete document;
        return;
    }

    QPdfWriter pdfWriter("");
    pdfWriter.setResolution(QGuiApplication::primaryScreen()->logicalDotsPerInch());
    QRectF pageSize = pdfWriter.pageLayout().paintRectPixels(pdfWriter.resolution());
    // Исходное изображение анкеты ("чистое")
    QImage image = document->page(0)->renderToImage(
        pdfWriter.resolution(), pdfWriter.resolution(), 0, 0, pageSize.width(), pageSize.height());
#elif Q_OS_WINDOWS
    QImage image(
        _dataDirPath + QDir::separator() + form->title() + QDir::separator() + form->title() +
        ".jpeg");
#endif
    if(image.isNull()) {
        // TODO ... error message ...
        return;
    }

    // открываем базу данных
    QString dbFileName = _dataDirPath + form->title() + QDir::separator() + "form.db";
    QSqlDatabase temp = QSqlDatabase::addDatabase("QSQLITE");
    temp.setHostName("NameDataBase");
    temp.setDatabaseName(dbFileName);
    if(!temp.open()) {
        qDebug() << "не открылась";
        return;
    }
    QSqlQuery query(temp);
    bool err = false;
    foreach(QString fileName, dirInf.entryList(QDir::Files)) {
        // тут часть во имя opencv

        QString pic = pathToPics + QDir::separator() + fileName;
        pic.remove("file://");
        qDebug() << pic;

        QString name = fileName.split('.').at(0);
        Mat imgMat = cv::imread(pic.toStdString());
        if(imgMat.empty()) {
            qDebug() << "Невозможно открыть изображение: " << fileName;
        } else {
            Mat binMat;
            cvtColor(imgMat, imgMat, cv::COLOR_RGBA2GRAY, 0);
            cv::adaptiveThreshold(
                imgMat, binMat, 200, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 2);

            // Переводим в QImage, потому что перевод из QImage в Mat хреново работает
            // Получаем бинаризованный binImg с отметками анкетируемого
            QImage binImg = MatToQImage(binMat);
            // Вычитаем из бинаризованного изображение исходное
            QImage result;
            substract(binImg, image, result);
            QString queryStr = "INSERT INTO NameTable ('ID Пациента', ";
            QString valuesStr = "VALUES (";
            foreach(ListItem* q, form->questions()->items()) {
                Question* qst = qobject_cast<Question*>(q);
                queryStr += tr("'Вопрос %1', ").arg(QString::number(qst->number()));
                QString answer = "";
                foreach(ListItem* a, qst->answers()->items()) {
                    Answer* ans = qobject_cast<Answer*>(a);
                    QRect findArea(
                        ans->coord().x() - LINE_SPACING,
                        ans->coord().y() - LINE_SPACING / 2,
                        ans->coord().width(),
                        ans->coord().height() + LINE_SPACING / 2);
                    QImage cropped = result.copy(findArea);
                    int blackPixels = 0;
                    for(int i = 0; i < cropped.height(); i++) {
                        QRgb* rgbLine = (QRgb*) cropped.constScanLine(i);
                        for(int j = 0; j < cropped.width(); j++) {
                            if(QColor(rgbLine[i]) == QColor("black"))
                                blackPixels++;
                        }
                    }
                    if(blackPixels > 50) {
                        // тут заносим в БД
                        if(answer.isEmpty())
                            answer += tr("%1").arg(ans->number());
                        else
                            answer += tr(", %1").arg(ans->number());
                    }
                }
                if(answer.isEmpty())
                    answer += tr("Нет ответа");
                valuesStr += tr("'%1', ").arg(answer);
            }
            valuesStr.remove(queryStr.length() - 2, 2);
            queryStr.remove(queryStr.length() - 2, 2);
            queryStr += tr(") %1);").arg(valuesStr);
            if(!query.exec(queryStr)) {
                qDebug() << "DataBase: error of insert " << query.lastQuery();
                temp.close();
                emit formProcessingProgress(100);
                err = true;
                break;
            }
        }
        progress++;
        emit formProcessingProgress(progress);
    }
    qDebug() << "обработка завершена";
    err ? emit formProcessingFinished("Ошибка записи в базу данных.") :
          emit formProcessingFinished("Обработка успешно завершена.");
    temp.close();
    delete document;
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

QImage Interface::MatToQImage(const Mat& mat) {
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type() == CV_8UC1) {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for(int i = 0; i < 256; i++)
            colorTable.push_back(qRgb(i, i, i));
        // Copy input Mat
        const uchar* qImageBuffer = (const uchar*) mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type() == CV_8UC3) {
        // Copy input Mat
        const uchar* qImageBuffer = (const uchar*) mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    } else {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

void Interface::substract(const QImage& left, const QImage& rigth, QImage& result) {
    int w = min(left.width(), rigth.width());
    int h = min(left.height(), rigth.height());
    w = min(w, result.width());
    h = min(h, result.height());
    //<-This ensures that you work only at the intersection of images areas

    for(int i = 0; i < h; i++) {
        QRgb* rgbLeft = (QRgb*) left.constScanLine(i);
        QRgb* rgbRigth = (QRgb*) rigth.constScanLine(i);
        QRgb* rgbResult = (QRgb*) result.constScanLine(i);
        for(int j = 0; j < w; j++) {
            rgbResult[j] = rgbLeft[j] - rgbRigth[j];
        }
    }
}
