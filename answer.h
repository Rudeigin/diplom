#ifndef ANSWER_H
#define ANSWER_H

#include <QObject>
#include <QPoint>

class Answer : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString aText READ aText WRITE setAText NOTIFY aTextChanged)
    //Номер или буква ответа
    Q_PROPERTY(QString number READ number WRITE setNumber NOTIFY numberChanged)
    Q_PROPERTY(QPoint coord READ coord WRITE setCoord NOTIFY coordChanged)
public:
    explicit Answer(QObject *parent = nullptr) : QObject(parent) {}
    Answer(const Answer &ans) : QObject(ans.parent()) {
        m_text = ans.m_text;
        m_number = ans.m_number;
        m_coord = ans.m_coord;
    }

    QString aText() const { return m_text; }
    QString number() const { return m_number; }
    QPoint coord() const { return m_coord; }

public slots:
    void setAText(QString text) {
        if (m_text == text)
            return;

        m_text = text;
        emit aTextChanged(m_text);
    }

    void setNumber(QString number) {
        if (m_number == number)
            return;

        m_number = number;
        emit numberChanged(m_number);
    }

    void setCoord(QPoint coord) {
        if (m_coord == coord)
            return;

        m_coord = coord;
        emit coordChanged(m_coord);
    }

signals:
    void aTextChanged(QString aText);
    void numberChanged(QString number);
    void coordChanged(QPoint coord);

private:
    QString m_text;
    QString m_number;
    QPoint m_coord;
};
Q_DECLARE_METATYPE(Answer)
#endif // ANSWER_H
