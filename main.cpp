#include <QApplication>
#include "smartweatherwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("SmartWeather");
    app.setApplicationDisplayName("SmartWeather");

    SmartWeatherWindow window;
    window.show();

    return app.exec();
}
