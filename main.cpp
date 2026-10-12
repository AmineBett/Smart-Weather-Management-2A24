#include <QApplication>
#include <QMainWindow>

#include "technicienne.h"
#include "stationwindow.h"

// Affiche la fenêtre "to" à la place de la fenêtre "from"
// en conservant la position, la taille et l'état maximisé.
static void basculerFenetre(QMainWindow *from, QMainWindow *to)
{
    const bool maximisee = from->isMaximized();

    if (!maximisee)
    {
        to->resize(from->size());
        to->move(from->pos());
    }

    if (maximisee)
        to->showMaximized();
    else
        to->show();

    to->raise();
    to->activateWindow();

    from->hide();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setStyle("Fusion");
    QApplication::setApplicationName("Smart Weather Management");

    // Les deux modules
    Technicienne fenetreTechniciens;
    StationWindow fenetreStations;

    // Technicienne -> clic sur "Gestion des stations"
    QObject::connect(&fenetreTechniciens, &Technicienne::stationsRequested,
                     &fenetreTechniciens,
                     [&fenetreTechniciens, &fenetreStations]()
                     {
                         basculerFenetre(&fenetreTechniciens, &fenetreStations);
                     });

    // Stations -> clic sur "Gestion des techniciens"
    QObject::connect(&fenetreStations, &StationWindow::techniciansRequested,
                     &fenetreStations,
                     [&fenetreTechniciens, &fenetreStations]()
                     {
                         basculerFenetre(&fenetreStations, &fenetreTechniciens);
                     });

    // L'interface des techniciens s'affiche en premier
    fenetreTechniciens.show();

    return app.exec();
}
