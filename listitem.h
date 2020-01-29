#ifndef LISTITEM_H
#define LISTITEM_H

#include <QDataStream>
#include <QObject>
#include <QVariantMap>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class ListItem: public QObject {
    Q_OBJECT
    Q_PROPERTY(QObject* item READ item CONSTANT)
    Q_PROPERTY(quint64 identifier READ id CONSTANT)
public:
    //! Конструктор по умолчанию
    ListItem(QObject* parent = 0): QObject(parent) {
        _id = quint64(this);
    }
    virtual ~ListItem() {
    }

    //! Вохвращает уникальный идентификатор объекта, который создается при создании объекта из его
    //! адреса в памяти
    Q_INVOKABLE quint64 id() const {
        return _id;
    }

    //! Заглушка метода для иницифлизации элемента модели из QVariantMap
    virtual void initFromMap(const QVariantMap& map) {
        Q_UNUSED(map)
    }

    //! Заглушка метода для генерации QVariantMap из объекта элемента модели
    virtual QVariantMap toMap() const {
        return QVariantMap();
    }

    //! Возвращает указатель на себя
    ListItem* item() {
        return this;
    }

    //! Заглушка метода валидации данных объекта.
    //! Реализуйте проверку необходимых данных в этом методе, если это требуется
    virtual bool validateItemData() const {
        return true;
    }

signals:
    //! Сигнал об изменении свойства (Q_PROPERTY)
    void dataChanged();

protected:
private:
    quint64 _id;
};

#endif // LISTITEM_H
