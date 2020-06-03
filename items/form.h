#ifndef FORM_H
#define FORM_H

#include "listmodel.h"
#include "question.h"

#include <QFont>
#include <QObject>

class Form: public ListItem {
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(ListModel* questions READ questions NOTIFY questionsChanged)
    Q_PROPERTY(QFont font READ font NOTIFY fontChanged)

public:
    explicit Form(QObject* parent = nullptr): ListItem(parent) {
        _title = "";
        _questions = new ListModel(this);
    }
    Form(const Form& frm): ListItem(frm.parent()) {
        _title = frm._title;
        _questions->initFromMap(frm.questions()->toMap());
    }

    ~Form() override {
        delete _questions;
    }

    void initFromMap(const QVariantMap& map) override {
        _title = map.value("title").toString();
        _questions->initFromMap(map.value("questions").toMap());
    }

    QVariantMap toMap() const override {
        QVariantMap map;
        map.insert("title", _title);
        map.insert("questions", _questions->toMap());
        return map;
    }

    QString title() const {
        return _title;
    }

    ListModel* questions() const {
        return _questions;
    }

    QFont font() const {
        return _font;
    }

    Question* addQuestion(int number = 0, QString text = "", bool pickAFew = false) {
        Question* qst = new Question(this);
        qst->setText(text);
        qst->setNumber(number);
        qst->setPickAFew(pickAFew);
        _questions->appendRow(qst);

        emit questionsChanged(_questions);
        emit dataChanged();

        return qst;
    }

public slots:
    void setTitle(QString title) {
        if(_title == title)
            return;

        _title = title;
        emit titleChanged(_title);
        emit dataChanged();
    }

signals:
    void titleChanged(QString title);
    void questionsChanged(ListModel* questions);

    void fontChanged(QFont* font);

private:
    QString _title;
    ListModel* _questions;
    QFont _font;
};

Q_DECLARE_METATYPE(Form)
#endif // FORM_H
