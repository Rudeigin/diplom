#include "interface.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    Interface* inf = new Interface();
    inf->addForm("1234");
    Form* f = qobject_cast<Form*>(inf->model()->row(0));
    f->addQuestion(0, "Вопрос 1", false);
    f->addQuestion(1, "Вопрос 2", false);
    inf->addForm("adf");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("Interface", inf);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if(engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
