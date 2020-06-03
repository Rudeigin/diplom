#ifndef TABLESQLMODEL_H
#define TABLESQLMODEL_H

#include <QObject>
#include <QSqlQueryModel>

class TableSqlModel: public QSqlQueryModel {
    Q_OBJECT
public:
    //! Конструктор по умолчанию
    explicit TableSqlModel(QObject* parent = 0);
    TableSqlModel(const TableSqlModel& model): TableSqlModel(model.parent()) {
    }

    //! Деструктор
    //    ~TableSqlModel();

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

protected:
    /* хешированная таблица ролей для колонок.
     * Метод используется в дебрях базового класса QAbstractItemModel,
     * от которого наследован класс QSqlQueryModel
     * */
    QHash<int, QByteArray> roleNames() const;
};

#endif // TABLESQLMODEL_H
