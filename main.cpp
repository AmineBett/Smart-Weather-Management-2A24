#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QList>
#include <QString>

#include "technicienne.h"
#include "stationwuindow.h"
#include "gbureaux.h"
#include "weatherwisetest.h"
#include "gutilisateur.h"

    // Switch from one module window to another
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

// Connect navigation buttons by their visible text
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

    // Create the five module windows
    Technicienne fenetreTechniciens;
    StationWindow fenetreStations;
    GBureaux fenetreBureaux;
    WeatherWiseTest fenetreEquipements;
    Gutilisateur fenetreUtilisateurs;

    // TECHNICIENS -> other modules
    connecterNavigation(&fenetreTechniciens,
                        &fenetreStations,
                        "Gestion des stations");

    connecterNavigation(&fenetreTechniciens,
                        &fenetreBureaux,
                        "bureau");

    connecterNavigation(&fenetreTechniciens,
                        &fenetreEquipements,
                        "Gestion des équipements");

    connecterNavigation(&fenetreTechniciens,
                        &fenetreUtilisateurs,
                        "Gestion des utilisateurs");

    // STATIONS -> other modules
    connecterNavigation(&fenetreStations,
                        &fenetreTechniciens,
                        "Gestion des techniciens");

    connecterNavigation(&fenetreStations,
                        &fenetreBureaux,
                        "bureau");

    connecterNavigation(&fenetreStations,
                        &fenetreEquipements,
                        "Gestion des équipements");

    connecterNavigation(&fenetreStations,
                        &fenetreUtilisateurs,
                        "Gestion des utilisateurs");

    // BUREAUX -> other modules
    connecterNavigation(&fenetreBureaux,
                        &fenetreTechniciens,
                        "Gestion des techniciens");

    connecterNavigation(&fenetreBureaux,
                        &fenetreStations,
                        "Gestion des stations");

    connecterNavigation(&fenetreBureaux,
                        &fenetreEquipements,
                        "Gestion des équipements");

    connecterNavigation(&fenetreBureaux,
                        &fenetreUtilisateurs,
                        "Gestion des utilisateurs");

    // EQUIPEMENTS -> other modules
    connecterNavigation(&fenetreEquipements,
                        &fenetreTechniciens,
                        "Gestion des techniciens");

    connecterNavigation(&fenetreEquipements,
                        &fenetreStations,
                        "Gestion des stations");

    connecterNavigation(&fenetreEquipements,
                        &fenetreBureaux,
                        "bureau");

    connecterNavigation(&fenetreEquipements,
                        &fenetreUtilisateurs,
                        "Gestion des utilisateurs");

    // UTILISATEURS -> other modules
    connecterNavigation(&fenetreUtilisateurs,
                        &fenetreTechniciens,
                        "Techniciens");

    connecterNavigation(&fenetreUtilisateurs,
                        &fenetreStations,
                        "Stations");

    connecterNavigation(&fenetreUtilisateurs,
                        &fenetreBureaux,
                        "bureau");

    connecterNavigation(&fenetreUtilisateurs,
                        &fenetreEquipements,
                        "Équipements");

    // Start with the Techniciens module
    fenetreTechniciens.show();

    return app.exec();
}