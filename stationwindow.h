#ifndef STATIONWINDOW_H
#define STATIONWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <QVector>
#include <QDate>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>

/*
 * Structure représentant une station météo
 */
struct Station
{
    int id;
    QString nom;
    QString localisation;
    double latitude;
    double longitude;
    QDate dateInstallation;
    QString type;
    QString etat;
};


/*
 * Widget personnalisé pour afficher
 * les statistiques sous forme de diagramme circulaire.
 */
class StatisticsWidget : public QWidget
{
public:
    explicit StatisticsWidget(QWidget *parent = nullptr);

    void setStatistics(int actifs,
                       int maintenance,
                       int horsService);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_actifs;
    int m_maintenance;
    int m_horsService;
};


/*
 * Fenêtre principale SmartWeather
 */
class StationWindow : public QMainWindow
{
public:
    explicit StationWindow(QWidget *parent = nullptr);

private:

    // =========================
    // Interface
    // =========================

    QWidget *centralWidget;

    // Champs du formulaire
    QSpinBox *idEdit;
    QLineEdit *nomEdit;
    QLineEdit *localisationEdit;

    QDoubleSpinBox *latitudeEdit;
    QDoubleSpinBox *longitudeEdit;

    QDateEdit *dateInstallationEdit;

    QComboBox *typeCombo;
    QComboBox *etatCombo;

    // Recherche / tri
    QLineEdit *searchEdit;
    QComboBox *sortCombo;

    // Tableau
    QTableWidget *table;

    // Statistiques
    StatisticsWidget *statisticsWidget;

    QLabel *totalLabel;

    // Données
    QVector<Station> stations;

    // =========================
    // Création de l'interface
    // =========================

    void setupUI();
    void setupStyle();

    QWidget *createSidebar();
    QWidget *createHeader();

    QGroupBox *createStationForm();
    QGroupBox *createActionBar();

    QGroupBox *createStationList();
    QGroupBox *createStatistics();
    QGroupBox *createLocationBox();

    // =========================
    // Données
    // =========================

    void loadDemoData();

    // =========================
    // Tableau
    // =========================

    void refreshTable();

    void updateStatistics();

    void clearForm();

    void fillFormFromSelectedRow();

    // =========================
    // Actions
    // =========================

    void addStation();
    void modifyStation();
    void deleteStation();

    void searchStations();
    void sortStations();

    void exportStations();

    // =========================
    // Utilitaires
    // =========================

    int selectedRow() const;

    QString statusText(const QString &etat) const;

    void updateLocationList();
};

#endif // STATIONWINDOW_H