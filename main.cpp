#include <QApplication>
#include <QFont>
#include "gbureaux.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setFont(QFont("Segoe UI", 10));

    GBureaux w;
    w.resize(1500, 900);
    w.showMaximized();      // plein écran : tout tient sans défilement
    return app.exec();
}