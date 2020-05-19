#ifndef INTERFACE_H
#define INTERFACE_H

#include "form.h"
#include "listmodel.h"

#include <QObject>

class Interface: public QObject {
    Q_OBJECT
    Q_PROPERTY(ListModel* model READ model NOTIFY modelChanged)
public:
    Interface(QObject* parent = nullptr);

    ListModel* model();
    Q_INVOKABLE Form* getForm(int index);
    Q_INVOKABLE void addForm(QString title);

signals:
    void modelChanged(ListModel* model);

private:
    ListModel* m_model;
};

#endif // INTERFACE_H
