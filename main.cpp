#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>

#include "Controller.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    app.setOrganizationName("No Organiztion");
    app.setOrganizationDomain("No Organiztion Domain Either");
    app.setWindowIcon(QIcon(":/images/icon.png"));

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    auto context = engine.rootContext();

    auto controller = new Controller;
    context->setContextProperty("controller", controller);

    return app.exec();
}
