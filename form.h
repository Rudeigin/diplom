#ifndef FORM_H
#define FORM_H

#include "listmodel.h"
#include "question.h"

#include <QObject>

class Form: public ListItem {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(ListModel* questions READ questions NOTIFY questionsChanged)

public:
    explicit Form(QObject* parent = nullptr): ListItem(parent) {
        m_title = "";
        m_questions = new ListModel(this);
    }
    Form(const Form& frm): ListItem(frm.parent()) {
        m_title = frm.m_title;
        m_questions->initFromMap(frm.questions()->toMap());
    }

    ~Form() {
        delete m_questions;
    }

    void initFromMap(const QVariantMap& map) override {
        m_title = map.value("title").toString();
        m_questions->initFromMap(map.value("questions").toMap());
    }

    QVariantMap toMap() const override {
        QVariantMap map;
        map.insert("title", m_title);
        map.insert("questions", m_questions->toMap());
        return map;
    }

    QString title() const {
        return m_title;
    }

    ListModel* questions() const {
        return m_questions;
    }

    Q_INVOKABLE void addQuestion(int number, QString text, bool pickAFew) {
        Question* qst = new Question(this);
        qst->setQText(text);
        qst->setNumber(number);
        qst->setPickAFew(pickAFew);
        m_questions->appendRow(qst);

        emit questionsChanged(m_questions);
        emit dataChanged();
    }

public slots:
    void setTitle(QString title) {
        if(m_title == title)
            return;

        m_title = title;
        emit titleChanged(m_title);
        emit dataChanged();
    }

signals:
    void titleChanged(QString title);
    void questionsChanged(ListModel* questions);

private:
    QString m_title;
    ListModel* m_questions;
};

Q_DECLARE_METATYPE(Form)
#endif // FORM_H
