#ifndef TECHNICIENNE_H
#define TECHNICIENNE_H

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QGroupBox>
#include <QDate>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>

class StatisticsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsWidget(QWidget *parent = nullptr);

    void setStatistics(int actifs, int inactifs);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int actifs;
    int inactifs;
};

class Technicienne : public QMainWindow
{
    Q_OBJECT

public:
    explicit Technicienne(QWidget *parent = nullptr);
    ~Technicienne();

private:

    // =========================
    // BARRE LATERALE
    // =========================

    QWidget *sidebar;

    QPushButton *btnAccueil;
    QPushButton *btnBureau;
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
    // RECHERCHE / TRI / IMPORT
    // =========================

    QLineEdit *recherche;
    QComboBox *triCombo;
    QPushButton *btnTrier;
    QPushButton *btnRechercher;
    QPushButton *btnImporterCV;


    // =========================
    // TABLE
    // =========================

    QTableWidget *table;


    // =========================
    // STATISTIQUES
    // =========================

    StatisticsWidget *statisticsWidget;

    QLabel *lblTotal;
    QLabel *lblActifs;
    QLabel *lblInactifs;


    // =========================
    // METHODES METIER
    // =========================

    QPushButton *btnEntretien;
    QPushButton *btnQuestionnaire;
    QPushButton *btnCompetences;
    QPushButton *btnSpecialite;


    // =========================
    // METHODES
    // =========================

    void setupUI();
    void setupSidebar();
    void setupTechnicienForm();
    void setupStatistics();
    void setupTable();
    void setupMetiers();

    void appliquerStyle();

    void ajouterTechnicien();
    void modifierTechnicien();
    void supprimerTechnicien();
    void viderChamps();

    void rechercherTechnicien();
    void trierTechnicien();
    void importerCV();

    void selectionnerTechnicien(int row, int column);

    void creerEntretien();
    void remplirQuestionnaire();
    void evaluerCompetences();
    void determinerSpecialite();

    void updateStatistics();
};

#endif // TECHNICIENNE_H
