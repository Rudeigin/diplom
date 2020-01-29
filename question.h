#ifndef QUESTION_H
#define QUESTION_H

#include "answer.h"
#include "listmodel.h"

#include <QObject>
#include <QPoint>

class Question: public ListItem {
    Q_OBJECT
    Q_PROPERTY(QString qText READ qText WRITE setQText NOTIFY qTextChanged)
    Q_PROPERTY(int number READ number WRITE setNumber NOTIFY numberChanged)
    Q_PROPERTY(bool pickAFew READ pickAFew WRITE setPickAFew NOTIFY pickAFewChanged)
    Q_PROPERTY(QPoint coord READ coord WRITE setCoord NOTIFY coordChanged)

public:
    explicit Question(QObject* parent = nullptr): ListItem(parent) {
        m_answers = new ListModel(this);
    }
    Question(const Question& qst): ListItem(qst.parent()) {
        m_text = qst.m_text;
        m_number = qst.m_number;
        m_pickAFew = qst.m_pickAFew;
        m_answers->initFromMap(qst.m_answers->toMap());
        m_coord = qst.m_coord;
    }

    ~Question() {
        delete m_answers;
    }

    void initFromMap(const QVariantMap& map) override {
        m_text = map.value("text").toString();
        m_number = map.value("number").toInt();
        m_pickAFew = map.value("pickAFew").toBool();
        m_answers->initFromMap(map.value("answers").toMap());
        m_coord = map.value("coord").toPoint();
    }

    QVariantMap toMap() const override {
        QVariantMap map;
        map.insert("text", m_text);
        map.insert("number", m_number);
        map.insert("pickAFew", m_pickAFew);
        map.insert("questions", m_answers->toMap());
        map.insert("coord", m_coord);
        return map;
    }

    QString qText() const {
        return m_text;
    }
    int number() const {
        return m_number;
    }
    bool pickAFew() const {
        return m_pickAFew;
    }
    Q_INVOKABLE ListModel* answers() const {
        return m_answers;
    }
    QPoint coord() const {
        return m_coord;
    }

    Q_INVOKABLE void addAnswer(QString number, QString text) {
        Answer* ans = new Answer(this);
        ans->setAText(text);
        ans->setNumber(number);
        m_answers->appendRow(ans);

        emit dataChanged();
    }

public slots:
    void setQText(QString text) {
        if(m_text == text)
            return;

        m_text = text;
        emit qTextChanged(m_text);
        emit dataChanged();
    }

    void setNumber(int number) {
        if(m_number == number)
            return;

        m_number = number;
        emit numberChanged(m_number);
        emit dataChanged();
    }

    void setPickAFew(bool pickAFew) {
        if(m_pickAFew == pickAFew)
            return;

        m_pickAFew = pickAFew;
        emit pickAFewChanged(m_pickAFew);
        emit dataChanged();
    }

    void setCoord(QPoint coord) {
        if(m_coord == coord)
            return;

        m_coord = coord;
        emit coordChanged(m_coord);
        emit dataChanged();
    }

signals:
    void qTextChanged(QString qText);
    void numberChanged(int number);
    void pickAFewChanged(bool pickAFew);
    void coordChanged(QPoint coord);

private:
    QString m_text;
    int m_number;
    bool m_pickAFew;
    ListModel* m_answers;
    QPoint m_coord;
};
Q_DECLARE_METATYPE(Question)
#endif // QUESTION_H
