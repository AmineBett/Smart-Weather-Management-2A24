#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QList>

#include "technicienne.h"
#include "stationwuindow.h"
#include "gbureaux.h"
#include "weatherwisetest.h"

static void basculerFenetre(QMainWindow *from, QMainWindow *to)
{
    if (from == nullptr || to == nullptr || from == to)
        return;

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

static void connecterNavigation(QMainWindow *from,
                                QMainWindow *to,
                                const QString &motCle)
{
    const QList<QPushButton *> boutons =
        from->findChildren<QPushButton *>();

    for (QPushButton *bouton : boutons)
    {
        if (bouton->text().contains(motCle, Qt::CaseInsensitive))
        {
            QObject::connect(
                bouton,
                &QPushButton::clicked,
                from,
                [from, to]()
                {
                    basculerFenetre(from, to);
                });
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setStyle("Fusion");
    QApplication::setApplicationName("Smart Weather Management");

    Technicienne fenetreTechniciens;
    StationWindow fenetreStations;
    GBureaux fenetreBureaux;
    WeatherWiseTest fenetreEquipements;

    // Navigation depuis Techniciens
    connecterNavigation(&fenetreTechniciens,
                        &fenetreStations,
                        "Gestion des stations");

    connecterNavigation(&fenetreTechniciens,
                        &fenetreBureaux,
                        "Gestion de bureau");

    connecterNavigation(&fenetreTechniciens,
                        &fenetreEquipements,
                        "Gestion des équipements");

    // Navigation depuis Stations
    connecterNavigation(&fenetreStations,
                        &fenetreTechniciens,
                        "Gestion des techniciens");

    connecterNavigation(&fenetreStations,
                        &fenetreBureaux,
                        "Gestion de bureau");

    connecterNavigation(&fenetreStations,
                        &fenetreEquipements,
                        "Gestion des équipements");

    // Navigation depuis Bureaux
    connecterNavigation(&fenetreBureaux,
                        &fenetreTechniciens,
                        "Gestion des techniciens");

    connecterNavigation(&fenetreBureaux,
                        &fenetreStations,
                        "Gestion des stations");

    connecterNavigation(&fenetreBureaux,
                        &fenetreEquipements,
                        "Gestion des équipements");

    // Navigation depuis Équipements
    connecterNavigation(&fenetreEquipements,
                        &fenetreTechniciens,
                        "Gestion des techniciens");

    connecterNavigation(&fenetreEquipements,
                        &fenetreStations,
                        "Gestion des stations");

    connecterNavigation(&fenetreEquipements,
                        &fenetreBureaux,
                        "Gestion de bureau");

    fenetreTechniciens.show();

    return app.exec();
}