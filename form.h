#ifndef FORM_H
#define FORM_H

#include "question.h"
#include <QObject>

class Form : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QList<Question *> questions READ questions WRITE setQuestions
               NOTIFY questionsChanged)

public:
    explicit Form(QObject *parent = nullptr) : QObject(parent) {}
    Form(const Form &frm) : QObject(frm.parent()) {
        m_title = frm.m_title;
        m_questions = frm.m_questions;
    }

    QString title() const { return m_title; }
    QList<Question *> questions() const { return m_questions; }

public slots:
    void setTitle(QString title) {
        if (m_title == title)
            return;

        m_title = title;
        emit titleChanged(m_title);
    }

    void setQuestions(QList<Question *> questions) {
        if (m_questions == questions)
            return;

        m_questions = questions;
        emit questionsChanged(m_questions);
    }

signals:
    void titleChanged(QString title);
    void questionsChanged(QList<Question *> questions);

private:
    QString m_title;
    QList<Question *> m_questions;
};

Q_DECLARE_METATYPE(Form)
#endif // FORM_H
