#include "interface.h"

#include <QDebug>

Interface::Interface(QObject* parent): QObject(parent) {
    qRegisterMetaType<Form>();
    qRegisterMetaType<Question>();
    qRegisterMetaType<Answer>();
    qRegisterMetaType<ListModel*>();

    m_model = new ListModel(this);
}

void Interface::addForm(QString title) {
    Form* frm = new Form(this);
    frm->setTitle(title);
    m_model->appendRow(frm);
    qDebug() << m_model->count();
}

ListModel* Interface::model() {
    return m_model;
}
