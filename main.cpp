#include "answer.h"
#include "form.h"
#include "question.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qRegisterMetaType<Form*>();
    qRegisterMetaType<Question*>();
    qRegisterMetaType<Answer*>();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if(engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
