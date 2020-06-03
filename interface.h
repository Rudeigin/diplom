#ifndef INTERFACE_H
#    define INTERFACE_H

#    include "items/form.h"
#    include "items/listmodel.h"

#    include <QFuture>
#    include <QObject>

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
    Q_INVOKABLE void addQst(Form* frm);
    Q_INVOKABLE void addAnsw(Question* qst);
    //    Q_INVOKABLE void deleteForm(int index);

    // основная логика
    /*
     * \brief Создание pdf-файла анкеты
     */
    Q_INVOKABLE void createPdf(Form* form);
    /*
     * \brief Запуск обработки результатов анкетирования
     */
    Q_INVOKABLE void processForms(Form* form, QString pathToPics);

signals:
    void modelChanged(ListModel* model);
    void formProcessingTotalCount(int count);
    void formProcessingProgress(int progress);
    void formProcessingFinished(QString result);

private:
    ListModel* _model;
    QString _formConfigName;
    QString _dataDirPath;
    QFuture<void> _future;

    // сохранение в json-файл
    void dumpForms();
    // загрузка из json-файл
    void loadForms();
    // обработка фото
    void processPics(Form* form, QString pathToPics);
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
