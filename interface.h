#ifndef INTERFACE_H
#define INTERFACE_H

#include "form.h"
#include "listmodel.h"

#include <QObject>

class Interface: public QObject {
    Q_OBJECT
public:
    Interface(QObject* parent = nullptr);

    Q_INVOKABLE ListModel* model();
    Q_INVOKABLE void addForm(QString title);

private:
    ListModel* m_model;
};

#endif // INTERFACE_H
