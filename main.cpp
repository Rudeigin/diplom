#include "interface.h"

#include <poppler/qt5/poppler-qt5.h>

#include <QApplication>
#include <QDebug>
#include <QImageReader>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    app.setOrganizationName("Some Company");
    app.setOrganizationDomain("somecompany.com");
    app.setApplicationName("Amazing Application");

    Interface* inf = new Interface(&app);
    //        inf->addForm("Оценка привычек в питании");
    //        Form* f = qobject_cast<Form*>(inf->model()->row(0));
    //        f->addQuestion(1, "Вопрос 1", false);
    //        Question* qst = qobject_cast<Question*>(f->questions()->row(0));
    //        qst->addAnswer("a", "Ответ 1");
    //        qst->addAnswer("б", "Ответ 2");
    //        f->addQuestion(2, "Вопрос 2", false);
    //        qst = qobject_cast<Question*>(f->questions()->row(1));
    //        qst->addAnswer("a", "Ответ 1");
    //        qst->addAnswer("б", "Ответ 2");
    //        inf->addForm("Определение риска ИБС");
    //        inf->addForm("Оценка физической формы");
    //        inf->addForm("Анкета для курильщиков");
    //        inf->createPdf(f);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("Interface", inf);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if(engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
