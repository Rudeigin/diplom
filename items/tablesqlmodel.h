#ifndef TABLESQLMODEL_H
#define TABLESQLMODEL_H

#include <QObject>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>

class TableSqlModel: public QSqlQueryModel {
    Q_OBJECT
    Q_PROPERTY(QString query READ queryStr WRITE setQueryStr NOTIFY queryStrChanged)
    Q_PROPERTY(QStringList userRoleNames READ userRoleNames CONSTANT)
public:
    using QSqlQueryModel::QSqlQueryModel;
    TableSqlModel(QObject* parent = nullptr);
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex& index, int role) const;
    QString queryStr() const;
    void setQueryStr(const QString& query);
    QStringList userRoleNames() const;
signals:
    void queryStrChanged();
};

#endif // TABLESQLMODEL_H
