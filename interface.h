#ifndef INTERFACE_H
#define INTERFACE_H

#include "items/form.h"
#include "items/listmodel.h"

#include <QObject>

class Interface: public QObject {
    Q_OBJECT
    // Модель со всеми анкетами
    Q_PROPERTY(ListModel* model READ model NOTIFY modelChanged)
public:
    Interface(QObject* parent = nullptr);

    ListModel* model();

    Q_INVOKABLE Form* getForm(int index);
    Q_INVOKABLE void addForm(QString title);
    //    Q_INVOKABLE void deleteForm(int index);

    Q_INVOKABLE void createPdf(Form* form);
//    Q_INVOKABLE void processForms(Form* form, QString pathToPics);

signals:
    void modelChanged(ListModel* model);

private:
    ListModel* m_model;
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
