#ifndef INTERFACE_H
#    define INTERFACE_H

#    include "items/form.h"
#    include "items/listmodel.h"
#    include "items/tablesqlmodel.h"

#    include <QFuture>
#    include <QObject>

#    include <opencv4/opencv2/core.hpp>
#    include <opencv4/opencv2/highgui.hpp>
#    include <opencv4/opencv2/imgproc.hpp>

class Interface: public QObject {
    Q_OBJECT
    // Модель со всеми анкетами
    Q_PROPERTY(ListModel* model READ model NOTIFY modelChanged)
public:
    Interface(QObject* parent = nullptr);
    ~Interface();

    ListModel* model();
    // вспомогательные функции для граф интерфейса
    Q_INVOKABLE Form* getForm(int index);
    Q_INVOKABLE Question* getQst(Form* form, int index);
    Q_INVOKABLE void addForm(QString title);
    Q_INVOKABLE void deleteForm(int index);
    Q_INVOKABLE void addQst(Form* frm);
    Q_INVOKABLE void deleteQst(Form* frm, int index);
    Q_INVOKABLE void addAnsw(Question* qst);
    Q_INVOKABLE void deleteAnsw(Question* qst, int index);
    Q_INVOKABLE void correl(Form* form);
    Q_INVOKABLE TableSqlModel* getSqlModel(Form* form);
    //    Q_INVOKABLE void deleteForm(int index);

    // основная логика
    /*
     * \brief Запуск создания pdf-файла анкеты
     */
    Q_INVOKABLE void createPdf(Form* form);
    /*
     * \brief Запуск обработки результатов анкетирования
     */
    Q_INVOKABLE void processForms(Form* form, QString pathToPics);

signals:
    void modelChanged(ListModel* model);
    void pdfCreatingFinished(QString result);
    void formProcessingTotalCount(int count);
    void formProcessingProgress(int progress);
    void formProcessingFinished(QString result);

private:
    ListModel* _model;
    QString _formConfigName;
    QString _dataDirPath;
    QFuture<void> _future;

    // создание пустой таблицы
    void createTable(Form* form);
    // сохранение в json-файл
    void dumpForms();
    // загрузка из json-файл
    void loadForms();
    // обработка фото
    void processPics(Form* form, QString pathToPics);
    // создание pdf
    void createFormPdf(Form* form);
    // https://stackoverflow.com/questions/11170485/qimage-to-cvmat-convertion-strange-behaviour
    QImage MatToQImage(const cv::Mat& mat);
    // https://stackoverflow.com/questions/43760920/how-to-compare-two-rgb-images-in-qt
    void substract(const QImage& left, const QImage& rigth, QImage& result);
};

#endif // INTERFACE_H

/*
 * 1. Необходимо добавить выбор папки с файлами
 * 2. Пока для одностраничных анкет: foreach на все файлы в папке
 * 3. Открываем картинку в opencv
 * 4. Преобразовываем анкету в jpg и открываем в openvcv
 * 5. Вычитаем изображения
 * 6. Ищем куски точек
 * 1) Можно пробегаться по областям ответа и считать предельное кол-во не нулей
 * можно поиграть с преобразованиями и светло-серый вывести в белый
 */
