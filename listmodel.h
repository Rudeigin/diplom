#ifndef LISTMODEL_H
#define LISTMODEL_H

#include "listitem.h"

#include <QAbstractListModel>
#include <QDataStream>
#include <QFile>
#include <QList>
#include <QVariant>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class ListModel: public QAbstractListModel {
    Q_OBJECT
    Q_ENUMS(ErrorCode)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum ErrorCode {
        NoError = 0,
        ErrorWhenOpenFile = -1,
        ErrorWhenParseFile = -2,
        ErrorWhenInitializing = -3
    };

    //! Конструктор по умолчанию
    explicit ListModel(QObject* parent = 0);
    ListModel(const ListModel& model): QAbstractListModel(model.parent()) {
    }

    //! Деструктор
    ~ListModel();

    //! Число элементов хранящихся в модели
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    //! Возвращает данные, соответствующие роли role, элемента, расположенного в index.row()
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    //! Возвращает QMap значений для всех ролей элемента, расположенного в index.row()
    virtual QMap<int, QVariant> itemData(const QModelIndex& index) const;

    //! Добавляет элемент в конец списка
    virtual void appendRow(ListItem* item);

    //! Добавляет список элементов в конец текущего списка модели
    virtual void appendRows(const QList<ListItem*>& items);

    //! Вставляет элемент в позицию row списка модели
    virtual void insertRow(int row, ListItem* item);

    //! Перемещает элемент из позиции sourceRow в позицию destinationRow
    virtual bool moveRow(int sourceRow, int destinationRow);

    //! Удаляет элемент из модели (сам элемент удаляется!)
    virtual bool removeRow(int row, const QModelIndex& parent = QModelIndex());

    //! Удаляет count элементов из модели, начиная с позиции row (сами элементы удаляются!)
    virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

    //! Удаляет элемент, находящийся в позиции row из модели и возвращает указатель на него (сам
    //! элемент не удаляется!)
    virtual ListItem* take(int row);

    //! Возвращает элемент, находящийся в позиции row
    virtual ListItem* row(int row);

    //! Возвращает все элементы хранящиеся в модели в виде списка
    virtual const QList<ListItem*>& items() const;

    //!
    virtual QList<ListItem*> find(const QVariant& value, const QString& propertyName);

    //! Ищет среди элементов модели такие, значение свойства propertyName, которых совпадает с
    //! value, удаляет их из модели и возвращает их список
    virtual QList<ListItem*> take(const QVariant& value, const QString& propertyName);

    //! Ищет среди элементов имеющих Q_PROPERTY = propertyName такие, значения данного свойства у
    //! которых совпадает с регэкспом value и возвращает список из них
    virtual QList<ListItem*> findByRegExp(const QVariant& value, const QString& propertyName);

    //! Возвращает флаги для элемента с индексом index
    virtual Qt::ItemFlags flags(const QModelIndex& index) const;

    //! Получает индекс элемента item
    virtual QModelIndex indexFromItem(const ListItem* item) const;

    //! Удаляет содержимое модели
    virtual void clear();

    //! Возвращает количество элементов модели
    Q_INVOKABLE virtual int count() const;

    //! Заглушка метода для иницифлизации модели из QVariantMap
    virtual ErrorCode initFromMap(const QVariantMap& map);

    //! Заглушка метода для генерации QVariantMap из объекта модели
    virtual QVariantMap toMap();

    virtual QHash<int, QByteArray> roleNames() const;

protected:
    QList<ListItem*> m_list;
    QHash<int, QByteArray> m_roles;
    int m_lastRoleIndex;

    //! Реализация создания объекта указанного класса
    virtual ListItem* createObject(const QString& className);

    //! Реализация удаления объекта указанного класса
    virtual void deleteObject(ListItem* item);

signals:

    //! Сигнал об изменении количества элементов в модели - используется в QML
    void countChanged(int newCount);

private slots:
    void handleItemChange();
    void updateRoleNames(const ListItem* item);
};

Q_DECLARE_METATYPE(ListModel)

#endif // LISTMODEL_H
