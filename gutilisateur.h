#ifndef GUTILISATEUR_H
#define GUTILISATEUR_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include <QString>
#include <QStringList>
#include <QStackedWidget>
#include <QRadioButton>
#include <QCheckBox>
#include <QDateTime>
#include <QWidget>
#include <QPainter>

// ============================================================
// GRAPHIQUE DONUT (statistiques par rôle)
// ============================================================
class DonutStats : public QWidget
{
public:
    DonutStats(QWidget *parent = 0) : QWidget(parent)
    {
        setMinimumSize(150, 150);
    }

    static QColor couleur(int i)
    {
        static const char *palette[] = {
            "#16A34A", "#E5484D", "#2563EB", "#9333EA",
            "#0891B2", "#DB2777", "#CA8A04", "#64748B"
        };
        return QColor(palette[i % 8]);
    }

    void setValeurs(const QList<int> &v)
    {
        valeurs = v;
        update();
    }

protected:
    void paintEvent(QPaintEvent *)
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        int total = 0;
        for (int i = 0; i < valeurs.size(); i++)
            total += valeurs.at(i);

        int cote = qMin(width(), height()) - 20;
        int ep = qMax(16, cote / 6);

        QRectF r((width() - cote) / 2.0 + ep / 2.0,
                 (height() - cote) / 2.0 + ep / 2.0,
                 cote - ep, cote - ep);

        QPen pen;
        pen.setWidth(ep);
        pen.setCapStyle(Qt::FlatCap);

        if (total == 0)
        {
            pen.setColor(QColor(0xE2E8F0));
            p.setPen(pen);
            p.drawArc(r, 0, 360 * 16);
        }
        else
        {
            double debut = 90 * 16;

            for (int i = 0; i < valeurs.size(); i++)
            {
                double span = 360.0 * 16 * valeurs.at(i) / total;

                pen.setColor(couleur(i));
                p.setPen(pen);
                p.drawArc(r, (int)debut, (int)(-span));

                debut -= span;
            }
        }

        QFont f = font();
        f.setPointSize(cote > 180 ? 24 : 18);
        f.setBold(true);
        p.setFont(f);
        p.setPen(QColor(0x0F2A52));
        p.drawText(QRectF(0, 0, width(), height() / 2.0 + 6),
                   Qt::AlignHCenter | Qt::AlignBottom,
                   QString::number(total));

        f.setPointSize(cote > 180 ? 11 : 9);
        f.setBold(false);
        p.setFont(f);
        p.setPen(QColor(0x334155));
        p.drawText(QRectF(0, height() / 2.0 + 6, width(), height() / 2.0),
                   Qt::AlignHCenter | Qt::AlignTop,
                   "utilisateurs");
    }

private:
    QList<int> valeurs;
};


// ============================================================
// FENETRE PRINCIPALE
// ============================================================
class Gutilisateur : public QMainWindow
{
    Q_OBJECT

public:
    explicit Gutilisateur(QWidget *parent = 0);
    ~Gutilisateur();

private slots:
    // connexion
    void seConnecter();
    void deconnecter();
    void ouvrirRecuperation();
    void retourConnexion();
    void majPlaceholderRecup();
    void envoyerCode();
    void reinitialiserMdp();

    // gestion des utilisateurs
    void ajouterUtilisateur();
    void modifierUtilisateur();
    void supprimerUtilisateur();
    void viderFormulaire();
    void majAccesFormulaire();
    void chargerSelection();
    void rechercherUtilisateur();
    void trierUtilisateurs();
    void afficherTous();
    void exporterPDF();
    void majHorloge();

private:

    struct Utilisateur
    {
        int id;
        QString nom;
        QString prenom;
        QString tel;
        QString role;
        QString mdp;
        QString email;
    };

    QList<Utilisateur> utilisateurs;
    QStringList rolesDisponibles;
    QStringList activites;

    QWidget *centralWidget;

    // ----- accueil / connexion -----
    QStackedWidget *pile;
    QStackedWidget *pileLogin;
    QFrame *pageLogin;
    QLineEdit *loginEmail;
    QLineEdit *loginMdp;
    QLabel *loginErreur;
    QPushButton *btnConnexion;
    int loginEchecs;
    int utilisateurConnecte;

    // ----- mot de passe oublié -----
    QRadioButton *radioEmail;
    QRadioButton *radioTel;
    QLineEdit *recupContact;
    QLineEdit *recupCode;
    QLineEdit *recupNouveau;
    QLineEdit *recupConfirm;
    QLabel *recupInfo1;
    QLabel *recupInfo2;
    int recupId;
    int recupEssais;
    QString recupCodeAttendu;
    QDateTime recupExpiration;

    // ----- en-tête -----
    QLabel *titre;
    QLabel *labelDate;
    QLabel *labelHeure;
    QLabel *labelConnecte;

    // ----- formulaire -----
    int idEnEdition;
    QLineEdit *champId;
    QLineEdit *champNom;
    QLineEdit *champPrenom;
    QLineEdit *champTel;
    QLineEdit *champEmail;
    QLineEdit *champMdp;
    QComboBox *champRole;
    QLabel *infoAcces;
    QLabel *erreur;

    // ----- barre d'outils + liste -----
    QPushButton *btnAjouter;
    QPushButton *btnModifier;
    QPushButton *btnSupprimer;
    QPushButton *btnVider;
    QPushButton *btnTrier;
    QPushButton *btnRechercher;
    QPushButton *btnExporter;
    QComboBox *tri;
    QLineEdit *recherche;
    QTableWidget *table;
    QLabel *labelNombre;

    // ----- statistiques / activité -----
    DonutStats *donut;
    QLabel *legendeStats;
    QLabel *labelActivite;

    // ----- menu -----
    QPushButton *btnDashboard;
    QPushButton *btnBureaux;
    QPushButton *btnEquipements;
    QPushButton *btnTechniciens;
    QPushButton *btnStations;
    QPushButton *btnUtilisateurs;

    void construireInterface();
    void construireLogin();
    void remplirTableau();
    void ajouterDonneesTest();
    void mettreAJourStatistiques();
    void enregistrerAction(const QString &action);

    void afficherMessage(const QString &texte, bool estErreur);
    bool lireFormulaire(Utilisateur &u, bool edition);
    void chargerPourModification(int id);
    int idSelectionne();
    int trouverUtilisateur(int id);
    bool aAcces(const QString &role);
    QString hacher(const QString &mdp);
};

#endif