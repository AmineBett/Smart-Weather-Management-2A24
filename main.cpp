#include <QApplication>
#include "stationwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("SmartWeather");
    app.setApplicationDisplayName("SmartWeather");

    StationWindow window;
    window.show();

    return app.exec();
}