#include "interface.h"

#include <QDebug>

Interface::Interface(QObject* parent): QObject(parent) {
    qRegisterMetaType<Form*>();
    qRegisterMetaType<Question*>();
    qRegisterMetaType<Answer*>();
    qRegisterMetaType<ListModel*>();

    m_model = new ListModel(this);
}

void Interface::addForm(QString title) {
    Form* frm = new Form(this);
    frm->setTitle(title);
    m_model->appendRow(frm);

    emit modelChanged(m_model);
}

ListModel* Interface::model() {
    return m_model;
}

Form* Interface::getForm(int index) {
    return qobject_cast<Form*>(m_model->row(index));
}
