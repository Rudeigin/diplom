#include "listmodel.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>
#include <QPointer>
#include <QRegExp>
#include <QStack>

#define MAGIC_NUMBER 50

namespace LMKey {
    const char ClassName[] = "className";
    const char Properties[] = "properties";
    const char Version[] = "version";
    const char Items[] = "items";
    const char VersionService[] = "$version";
} // namespace LMKey

ListModel::ListModel(QObject* parent): QAbstractListModel(parent) {
    m_roles[Qt::UserRole + 2] = "item";
    m_roles[Qt::UserRole + 3] = "typeName";
    m_roles[Qt::UserRole + 4] = "identifier";
    m_lastRoleIndex = Qt::UserRole + 5;
}

QHash<int, QByteArray> ListModel::roleNames() const {
    return m_roles;
}

ListItem* ListModel::createObject(const QString& className) {
    int type = QMetaType::type(className.toLocal8Bit());
    ListItem* ptr = NULL;
    if(type != 0) {
        ptr = (ListItem*) (QMetaType::create(type));
    }
    return ptr;
}

void ListModel::deleteObject(ListItem* item) {
    if(item) {
        item->deleteLater();
    }
}

int ListModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return m_list.size();
}

QVariant ListModel::data(const QModelIndex& index, int role) const {
    if(index.row() < 0 || index.row() >= m_list.size())
        return QVariant();
    switch(role) {
        case Qt::UserRole + 2:
            return QVariant::fromValue(qobject_cast<QObject*>(m_list.at(index.row())->item()));
            break;
        case Qt::UserRole + 3:
            return m_list.at(index.row())->metaObject()->className();
            break;
        case Qt::UserRole + 4:
            return m_list.at(index.row())->id();
            break;
        case Qt::DisplayRole:
            return (qobject_cast<QObject*>(m_list.at(index.row())->item()))->objectName();
            break;
        default:
            /*if( m_list.at(index.row())->hasCustomRoles())
        {*/
            if(role >= Qt::UserRole + 5 && role < m_lastRoleIndex) {
                const QByteArray& roleName = m_roles.value(role);
                QVariant value = m_list.at(index.row())->item()->property(roleName);
                return value;
            } else
                return QVariant();
            /*}
        else
        return QVariant();*/
    }
    return QVariant();
}

ListModel::~ListModel() {
    //! \todo !!! Большой вопрос необходимо ли вызывать clear, т.к. в списках лежат кнопки родитель
    //! которых - объект QObject.
    // clear();
}

void ListModel::appendRow(ListItem* item) {
    appendRows(QList<ListItem*>() << item);
}

void ListModel::appendRows(const QList<ListItem*>& items) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount() + items.size() - 1);
    foreach(ListItem* item, items) {
        connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
        updateRoleNames(item);
        m_list.append(item);
    }
    endInsertRows();
    emit countChanged(m_list.size());
}

void ListModel::insertRow(int row, ListItem* item) {
    beginInsertRows(QModelIndex(), row, row);
    connect(item, SIGNAL(dataChanged()), SLOT(handleItemChange()));
    updateRoleNames(item);
    m_list.insert(row, item);
    endInsertRows();
    emit countChanged(m_list.size());
}

bool ListModel::moveRow(int sourceRow, int destinationRow) {
    if(sourceRow == destinationRow || sourceRow < 0 || destinationRow < 0 ||
       m_list.count() <= sourceRow || m_list.count() <= destinationRow) {
        return false;
    }
    // ++ @see QTBUG-13628
    if(sourceRow + 1 == destinationRow || sourceRow + 2 == destinationRow) {
        ListItem* item = take(sourceRow);
        insertRow(destinationRow, item);
        return true;
    }
    // -- QTBUG-13628

    if(beginMoveRows(
           index(sourceRow, 0), sourceRow, sourceRow, index(destinationRow, 0), destinationRow)) {
        m_list.move(sourceRow, destinationRow);
        endMoveRows();
        emit countChanged(m_list.size());
        return true;
    }
    return false;
}

void ListModel::handleItemChange() {
    ListItem* item = static_cast<ListItem*>(sender());
    QModelIndex index = indexFromItem(item);
    if(index.isValid()) {
        emit dataChanged(index, index);
    }
}

QList<ListItem*> ListModel::take(const QVariant& value, const QString& propertyName) {
    int row = 0;
    QList<ListItem*> result;
    while(row < m_list.size()) {
        if(value == m_list.at(row)->property(propertyName.toLocal8Bit()))
            result.append(take(row));
        else
            row++;
    }
    return result;
}
QList<ListItem*> ListModel::find(const QVariant& value, const QString& propertyName) {
    QList<ListItem*> result;
    foreach(ListItem* item, m_list) {
        if(value == item->property(propertyName.toLocal8Bit())) {
            result.append(item);
        }
    }
    return result;
}

QList<ListItem*> ListModel::findByRegExp(const QVariant& value, const QString& propertyName) {
    QList<ListItem*> result;
    foreach(ListItem* item, m_list) {
        QString pattern = "^" + item->property(propertyName.toLocal8Bit()).toString();
        pattern = pattern.replace(".", "*");
        pattern = pattern.replace("x", "\\d");
        QRegExp regExp;
        regExp.setPatternSyntax(QRegExp::RegExp);
        regExp.setPattern(pattern);
        if(regExp.isValid() && pattern != "^") {
            if(regExp.indexIn(value.toString(), 0) != -1) {
                result.append(item);
            }
        }
    }
    return result;
}

QModelIndex ListModel::indexFromItem(const ListItem* item) const {
    Q_ASSERT(item);
    for(int row = 0; row < m_list.size(); ++row) {
        if(m_list.at(row) == item)
            return index(row);
    }
    return QModelIndex();
}

void ListModel::clear() {
    if(m_list.size() > 0) {
        beginRemoveRows(QModelIndex(), 0, m_list.size() - 1);
        foreach(ListItem* item, m_list) { deleteObject(item); }
        // qDeleteAll(m_list.begin(),m_list.end());
        m_list.clear();
        endRemoveRows();
        emit countChanged(m_list.size());
    }
}

bool ListModel::removeRow(int row, const QModelIndex& parent) {
    Q_UNUSED(parent);
    if(row < 0 || row >= m_list.size())
        return false;
    beginRemoveRows(QModelIndex(), row, row);
    deleteObject(m_list.takeAt(row));
    endRemoveRows();
    emit countChanged(m_list.size());
    return true;
}

bool ListModel::removeRows(int row, int count, const QModelIndex& parent) {
    Q_UNUSED(parent);
    if(row < 0 || (row + count) > m_list.size())
        return false;
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for(int i = 0; i < count; ++i) {
        deleteObject(m_list.takeAt(row));
    }
    endRemoveRows();
    emit countChanged(m_list.size());
    return true;
}

ListItem* ListModel::take(int row) {
    beginRemoveRows(QModelIndex(), row, row);
    ListItem* item = m_list.takeAt(row);
    endRemoveRows();
    emit countChanged(m_list.size());
    return item;
}

Qt::ItemFlags ListModel::flags(const QModelIndex& index) const {
    if(!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QMap<int, QVariant> ListModel::itemData(const QModelIndex& index) const {
    QMap<int, QVariant> data;
    if(index.row() < 0 || index.row() >= m_list.size())
        return QMap<int, QVariant>();
    ListItem* item = m_list.at(index.row());
    data[Qt::UserRole + 2] = QVariant::fromValue(qobject_cast<QObject*>(item));
    data[Qt::UserRole + 3] = item->metaObject()->className();
    data[Qt::UserRole + 4] = item->id();
    QHashIterator<int, QByteArray> i(roleNames());
    while(i.hasNext()) {
        i.next();
        QVariant value = item->property(i.value());
        if(value.isValid())
            data.insert(i.key(), value);
    }
    return data;
}

int ListModel::count() const {
    return m_list.size();
}

ListModel::ErrorCode ListModel::initFromMap(const QVariantMap& map) {
    clear();

    int version = -1;
    if(map.contains(LMKey::Version)) {
        version = map.value(LMKey::Version, -1).toInt();
    }
    // check service info
    if(version < 0) {
        qWarning() << Q_FUNC_INFO << "map is invalid. Model won't be configured. Map: " << map;
        return ErrorWhenInitializing;
    }
    QList<ListItem*> items;
    if(map.contains(LMKey::Items)) {
        QVariantList itemsList = map.value(LMKey::Items).toList();
        foreach(QVariant itemVariant, itemsList) {
            QVariantMap itemMap = itemVariant.toMap();
            if(itemMap.contains(LMKey::ClassName)) {
                QString className = itemMap.value(LMKey::ClassName).toString();
                if(!className.isEmpty()) {
                    ListItem* itemObject = createObject(className);
                    if(itemObject != 0) {
                        itemObject->setParent(this);

                        if(itemMap.contains(LMKey::Properties)) {
                            QVariantMap itemProperties = itemMap.value(LMKey::Properties).toMap();
                            itemProperties.insert(LMKey::VersionService, version);
                            itemObject->initFromMap(itemProperties);
                        }
                        if(itemObject->validateItemData()) {
                            items.append(itemObject);
                        } else {
                            qWarning()
                                << Q_FUNC_INFO << itemObject << "is invalid. Item will be removed";
                            deleteObject(itemObject);
                        }
                    }
                }
            }
        }
    }
    appendRows(items);
    return NoError;
}

QVariantMap ListModel::toMap() {
    QVariantList items;
    foreach(ListItem* ptr, m_list) {
        QString className = QString::fromLocal8Bit(ptr->metaObject()->className());
        QVariantMap itemMap;
        itemMap.insert(LMKey::ClassName, className);
        itemMap.insert(LMKey::Properties, ptr->toMap());
        items.append(itemMap);
    }
    QVariantMap modelMap;
    modelMap.insert(LMKey::Version, MAGIC_NUMBER);
    modelMap.insert(LMKey::Items, items);
    return modelMap;
}

ListItem* ListModel::row(int row) {
    if(row >= 0 && row < m_list.size()) {
        return m_list.at(row);
    } else
        return 0;
}

const QList<ListItem*>& ListModel::items() const {
    return m_list;
}

void ListModel::updateRoleNames(const ListItem* item) {
    const QMetaObject* metaObject = item->metaObject();
    while(metaObject) {
        for(int i = metaObject->propertyOffset() + metaObject->propertyCount() - 1;
            i >= metaObject->propertyOffset();
            --i) {
            QByteArray propertyName = metaObject->property(i).name();
            if(!m_roles.values().contains(propertyName))
                m_roles.insert(m_lastRoleIndex++, propertyName);
        }
        metaObject = metaObject->superClass();
        if((metaObject != 0) && (qstrncmp(metaObject->className(), "ListItem", 8) == 0))
            metaObject = metaObject->superClass();
    }
}
