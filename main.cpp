#include <QApplication>
#include "technicienne.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setStyle("Fusion");

    Technicienne window;

    window.show();

    return app.exec();
}
