#ifndef QUESTION_H
#define QUESTION_H

#include "answer.h"
#include <QObject>
#include <QPoint>

class Question : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString qText READ qText WRITE setQText NOTIFY qTextChanged)
    Q_PROPERTY(int number READ number WRITE setNumber NOTIFY numberChanged)
    Q_PROPERTY(
            bool pickAFew READ pickAFew WRITE setPickAFew NOTIFY pickAFewChanged)
    Q_PROPERTY(QList<Answer *> answers READ answers WRITE setAnswers NOTIFY
               answersChanged)
    Q_PROPERTY(QPoint coord READ coord WRITE setCoord NOTIFY coordChanged)

public:
    explicit Question(QObject *parent = nullptr) : QObject(parent) {}
    Question(const Question &qst) : QObject(qst.parent()) {
        m_text = qst.m_text;
        m_number = qst.m_number;
        m_pickAFew = qst.m_pickAFew;
        m_answers = qst.m_answers;
        m_coord = qst.m_coord;
    }

    QString qText() const { return m_text; }
    int number() const { return m_number; }
    bool pickAFew() const { return m_pickAFew; }
    QList<Answer *> answers() const { return m_answers; }
    QPoint coord() const { return m_coord; }

public slots:
    void setQText(QString text) {
        if (m_text == text)
            return;

        m_text = text;
        emit qTextChanged(m_text);
    }

    void setNumber(int number) {
        if (m_number == number)
            return;

        m_number = number;
        emit numberChanged(m_number);
    }

    void setPickAFew(bool pickAFew) {
        if (m_pickAFew == pickAFew)
            return;

        m_pickAFew = pickAFew;
        emit pickAFewChanged(m_pickAFew);
    }

    void setAnswers(QList<Answer *> answers) {
        if (m_answers == answers)
            return;

        m_answers = answers;
        emit answersChanged(m_answers);
    }

    void setCoord(QPoint coord) {
        if (m_coord == coord)
            return;

        m_coord = coord;
        emit coordChanged(m_coord);
    }

signals:
    void qTextChanged(QString qText);
    void numberChanged(int number);
    void pickAFewChanged(bool pickAFew);
    void answersChanged(QList<Answer *> answers);
    void coordChanged(QPoint coord);

private:
    QString m_text;
    int m_number;
    bool m_pickAFew;
    QList<Answer *> m_answers;
    QPoint m_coord;
};
Q_DECLARE_METATYPE(Question)
#endif // QUESTION_H
