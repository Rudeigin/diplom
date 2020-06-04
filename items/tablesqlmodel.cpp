#include "tablesqlmodel.h"

TableSqlModel::TableSqlModel(QObject* parent): QSqlQueryModel(parent) {
}

QHash<int, QByteArray> TableSqlModel::roleNames() const {
    QHash<int, QByteArray> roles;
    for(int i = 0; i < record().count(); i++) {
        roles.insert(Qt::UserRole + i + 1, record().fieldName(i).toUtf8());
    }
    return roles;
}
QVariant TableSqlModel::data(const QModelIndex& index, int role) const {
    QVariant value;
    if(index.isValid()) {
        if(role < Qt::UserRole) {
            value = QSqlQueryModel::data(index, role);
        } else {
            int columnIdx = role - Qt::UserRole - 1;
            QModelIndex modelIndex = this->index(index.row(), columnIdx);
            value = QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
        }
    }
    return value;
}
QString TableSqlModel::queryStr() const {
    return query().lastQuery();
}
void TableSqlModel::setQueryStr(const QString& query) {
    if(queryStr() == query)
        return;
    setQuery(query);
    emit queryStrChanged();
}
QStringList TableSqlModel::userRoleNames() const {
    QStringList names;
    for(int i = 0; i < record().count(); i++) {
        names << record().fieldName(i).toUtf8();
    }
    return names;
}
