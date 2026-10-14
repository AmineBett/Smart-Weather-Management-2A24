#ifndef TECHNICIENNE_H
#define TECHNICIENNE_H

#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QGroupBox>
#include <QString>

/*
 * Widget de statistiques des techniciens (diagramme circulaire).
 * Renommé TechStatisticsWidget pour éviter tout conflit avec la classe
 * StatisticsWidget du module "Gestion des stations".
 */
class TechStatisticsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TechStatisticsWidget(QWidget *parent = nullptr);

    void setStatistics(int actifs, int inactifs);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_actifs;
    int m_inactifs;
};

class Technicienne : public QMainWindow
{
    Q_OBJECT

public:
    explicit Technicienne(QWidget *parent = nullptr);
    ~Technicienne() override;

signals:
    // Émis lorsque l'utilisateur clique sur "Gestion des stations"
    void stationsRequested();

private:

    // =========================
    // BARRE LATERALE
    // =========================

    QWidget *sidebar;

    QPushButton *btnAccueil;
    QPushButton *btnBureau;        // bouton "Paramètres" (bas de la barre)
    QPushButton *btnEquipements;
    QPushButton *btnTechniciens;
    QPushButton *btnStations;
    QPushButton *btnUtilisateurs;


    // =========================
    // CHAMPS TECHNICIEN
    // =========================

    QLineEdit *idTechnicien;
    QLineEdit *nom;
    QLineEdit *prenom;
    QLineEdit *email;
    QLineEdit *telephone;
    QLineEdit *formation;
    QSpinBox *experience;
    QLineEdit *specialite;
    QTextEdit *competences;
    QComboBox *statut;
    QDateEdit *dateIntegration;


    // =========================
    // BOUTONS CRUD
    // =========================

    QPushButton *btnAjouter;
    QPushButton *btnModifier;
    QPushButton *btnSupprimer;
    QPushButton *btnVider;


    // =========================
    // RECHERCHE / TRI / EXPORT PDF
    // =========================

    QLineEdit *recherche;
    QComboBox *triCombo;
    QPushButton *btnTrier;
    QPushButton *btnRechercher;
    QPushButton *btnExporterPDF;


    // =========================
    // TABLE
    // =========================

    QTableWidget *table;


    // =========================
    // STATISTIQUES
    // =========================

    TechStatisticsWidget *statisticsWidget;

    QLabel *lblTotal;
    QLabel *lblActifs;
    QLabel *lblInactifs;


    // =========================
    // METHODES
    // =========================

    void setupUI();
    QWidget *setupSidebar();
    QWidget *setupHeader();
    QGroupBox *setupTechnicienForm();
    QWidget *setupActionBar();
    QGroupBox *setupStatistics();
    QGroupBox *setupTable();

    void appliquerStyle();
    void chargerDonneesDemo();

    void ajouterTechnicien();
    void modifierTechnicien();
    void supprimerTechnicien();
    void viderChamps();

    void rechercherTechnicien();
    void trierTechnicien();
    void exporterPDF();

    void selectionnerTechnicien(int row, int column);

    void updateStatistics();

    // Utilitaires
    bool validerChamps(int ligneIgnoree);
    int trouverLigneParId(int id) const;
    void remplirLigne(int row);
};

#endif // TECHNICIENNE_H
