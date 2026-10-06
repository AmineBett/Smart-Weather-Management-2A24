#include "gutilisateur.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Gutilisateur w;

    w.show();

    return a.exec();
}