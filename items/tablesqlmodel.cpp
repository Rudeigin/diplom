#include "tablesqlmodel.h"

QVariant TableSqlModel::data(const QModelIndex& index, int role) const {
    // Определяем номер колонки по номеру роли
    int columnId = role - Qt::UserRole - 1;
    // Создаём индекс с помощью новоиспечённого ID колонки
    QModelIndex modelIndex = this->index(index.row(), columnId);

    return QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
}

QHash<int, QByteArray> TableSqlModel::roleNames() const {
}
