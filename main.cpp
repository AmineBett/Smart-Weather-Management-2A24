#include <QApplication>
#include <QFont>
#include "weatherwisetest.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setFont(
        QFont("Segoe UI", 10)
        );

    WeatherWiseTest w;

    w.resize(
        1500,
        900
        );

    w.showMaximized();

    return app.exec();
}