#ifndef ANSWER_H
#define ANSWER_H

#include <QObject>
#include <QPoint>

class Answer : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
  //Номер или буква ответа
  Q_PROPERTY(QString number READ number WRITE setNumber NOTIFY numberChanged)
  Q_PROPERTY(QPoint coord READ coord WRITE setCoord NOTIFY coordChanged)
public:
  explicit Answer(QObject *parent = nullptr) : QObject(parent) {}

  QString text() const { return m_text; }
  QString number() const { return m_number; }
  QPoint coord() const { return m_coord; }

public slots:
  void setText(QString text) {
    if (m_text == text)
      return;

    m_text = text;
    emit textChanged(m_text);
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
  void textChanged(QString text);
  void numberChanged(QString number);
  void coordChanged(QPoint coord);

private:
  QString m_text;
  QString m_number;
  QPoint m_coord;
};
Q_DECLARE_METATYPE(Answer)
#endif // ANSWER_H
