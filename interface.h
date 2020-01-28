#ifndef INTERFACE_H
#define INTERFACE_H

#include <QObject>
#include "form.h"

class Interface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<Form*> model READ model WRITE setModel NOTIFY modelChanged)
public:
    explicit Interface(QObject *parent = nullptr);

signals:

public slots:

private:
    QList<Form*> m_model;
};

#endif // INTERFACE_H
