#ifndef ANSWER_H
#define ANSWER_H

#include "listitem.h"

#include <QObject>
#include <QRect>

class Answer: public ListItem {
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    //Номер или буква ответа
    Q_PROPERTY(QString number READ number WRITE setNumber NOTIFY numberChanged)
    Q_PROPERTY(QRect coord READ coord WRITE setCoord NOTIFY coordChanged)
public:
    explicit Answer(QObject* parent = nullptr): ListItem(parent) {
        m_coord = QRect();
        m_number = "0";
        m_text = "";
    }
    Answer(const Answer& ans): ListItem(ans.parent()) {
        m_text = ans.m_text;
        m_number = ans.m_number;
        m_coord = ans.m_coord;
    }

    void initFromMap(const QVariantMap& map) override {
        m_text = map.value("text").toString();
        m_number = map.value("number").toString();
        m_coord = map.value("coord").toRect();
    }

    QVariantMap toMap() const override {
        QVariantMap map;
        map.insert("text", m_text);
        map.insert("number", m_number);
        map.insert("coord", m_coord);
        return map;
    }

    QString text() const {
        return m_text;
    }
    QString number() const {
        return m_number;
    }
    QRect coord() const {
        return m_coord;
    }

public slots:
    void setText(QString text) {
        if(m_text == text)
            return;

        m_text = text;
        emit textChanged(m_text);
        emit dataChanged();
    }

    void setNumber(QString number) {
        if(m_number == number)
            return;

        m_number = number;
        emit numberChanged(m_number);
        emit dataChanged();
    }

    void setCoord(QRect coord) {
        if(m_coord == coord)
            return;

        m_coord = coord;
        emit coordChanged(m_coord);
        emit dataChanged();
    }

signals:
    void textChanged(QString text);
    void numberChanged(QString number);
    void coordChanged(QRect coord);

private:
    QString m_text;
    QString m_number;
    QRect m_coord;
};
Q_DECLARE_METATYPE(Answer)
#endif // ANSWER_H
