#include "gutilisateur.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QMessageBox>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QPrinter>
#include <QTextDocument>
#include <QDir>
#include <QCryptographicHash>
#include <QTimer>
#include <QTime>
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#endif
#include <algorithm>

// ============================================================
// CONSTRUCTEUR / DESTRUCTEUR
// ============================================================

Gutilisateur::Gutilisateur(QWidget *parent)
    : QMainWindow(parent)
{
    idEnEdition = -1;
    utilisateurConnecte = -1;
    loginEchecs = 0;
    recupId = -1;
    recupEssais = 0;
    labelActivite = 0;

    // Liste des rôles (modifie-la ici si besoin)
    rolesDisponibles << "Administrateur"
                     << "Responsable RH"
                     << "Technicien"
                     << "Météorologue"
                     << "Ingénieur"
                     << "Responsable de station";

    construireInterface();
    ajouterDonneesTest();
    trierUtilisateurs();
    mettreAJourStatistiques();
    viderFormulaire();
}

Gutilisateur::~Gutilisateur()
{
}


// ============================================================
// INTERFACE
// ============================================================

void Gutilisateur::construireInterface()
{
    setWindowTitle("Smart Weather Management - Gestion des utilisateurs");
    setMinimumSize(1200, 660);
    resize(1366, 740);
    setWindowState(Qt::WindowMaximized);

    pile = new QStackedWidget(this);
    setCentralWidget(pile);

    construireLogin();                 // page 0 : accueil / connexion

    centralWidget = new QWidget();     // page 1 : application
    pile->addWidget(centralWidget);
    pile->setCurrentIndex(0);

    QHBoxLayout *principal = new QHBoxLayout(centralWidget);
    principal->setContentsMargins(0, 0, 0, 0);
    principal->setSpacing(0);


    // ========================================================
    // MENU GAUCHE
    // ========================================================

    QFrame *menu = new QFrame();
    menu->setObjectName("menu");
    menu->setFixedWidth(235);

    QVBoxLayout *lm = new QVBoxLayout(menu);
    lm->setContentsMargins(12, 28, 12, 15);
    lm->setSpacing(6);

    QLabel *soleil = new QLabel("☀");
    soleil->setObjectName("logoSoleil");
    soleil->setAlignment(Qt::AlignCenter);

    QLabel *logoTexte = new QLabel("SMART WEATHER\nMANAGEMENT");
    logoTexte->setObjectName("logoTexte");
    logoTexte->setAlignment(Qt::AlignCenter);

    lm->addWidget(soleil);
    lm->addWidget(logoTexte);
    lm->addSpacing(30);

    btnDashboard    = new QPushButton("⌂   Accueil");
    btnBureaux      = new QPushButton("🏢   Gestion de bureau");
    btnEquipements  = new QPushButton("⚙   Gestion des équipements");
    btnTechniciens  = new QPushButton("👷   Gestion des techniciens");
    btnStations     = new QPushButton("📡   Gestion des stations");
    btnUtilisateurs = new QPushButton("👤   Gestion des utilisateurs");

    btnUtilisateurs->setCheckable(true);
    btnUtilisateurs->setChecked(true);

    lm->addWidget(btnDashboard);
    lm->addWidget(btnBureaux);
    lm->addWidget(btnEquipements);
    lm->addWidget(btnTechniciens);
    lm->addWidget(btnStations);
    lm->addWidget(btnUtilisateurs);
    lm->addStretch();

    principal->addWidget(menu);


    // ========================================================
    // COLONNE DROITE (en-tête + corps)
    // ========================================================

    QWidget *droite = new QWidget();
    QVBoxLayout *ld = new QVBoxLayout(droite);
    ld->setContentsMargins(0, 0, 0, 0);
    ld->setSpacing(0);


    // ---------- EN-TETE ----------
    QFrame *entete = new QFrame();
    entete->setObjectName("entete");
    entete->setFixedHeight(72);

    QHBoxLayout *le = new QHBoxLayout(entete);
    le->setContentsMargins(30, 10, 30, 10);
    le->setSpacing(18);

    QVBoxLayout *textes = new QVBoxLayout();
    titre = new QLabel("Gestion des utilisateurs");
    titre->setObjectName("titrePage");
    QLabel *sousTitre = new QLabel("Gérez les utilisateurs, leurs rôles et leurs accès");
    sousTitre->setObjectName("sousTitre");
    textes->addStretch();
    textes->addWidget(titre);
    textes->addWidget(sousTitre);
    textes->addStretch();

    labelDate = new QLabel();
    labelDate->setObjectName("enteteInfo");

    labelHeure = new QLabel();
    labelHeure->setObjectName("enteteInfo");

    labelConnecte = new QLabel("•  Administrateur");
    labelConnecte->setObjectName("enteteInfo");

    QPushButton *btnDeconnexion = new QPushButton("⏻  Déconnexion");
    btnDeconnexion->setObjectName("btnDeconnexion");
    btnDeconnexion->setCursor(Qt::PointingHandCursor);
    connect(btnDeconnexion, SIGNAL(clicked()), this, SLOT(deconnecter()));

    le->addLayout(textes);
    le->addStretch();
    le->addWidget(labelDate);
    le->addWidget(labelHeure);
    le->addWidget(labelConnecte);
    le->addWidget(btnDeconnexion);

    QTimer *horloge = new QTimer(this);
    connect(horloge, SIGNAL(timeout()), this, SLOT(majHorloge()));
    horloge->start(1000);
    majHorloge();

    ld->addWidget(entete);


    // ---------- CORPS (tout tient sur une seule page) ----------
    QFrame *corps = new QFrame();
    corps->setObjectName("corps");

    QVBoxLayout *lc = new QVBoxLayout(corps);
    lc->setContentsMargins(20, 12, 20, 12);
    lc->setSpacing(10);


    // ========================================================
    // FORMULAIRE (en haut, toujours visible)
    // ========================================================

    QFrame *carteForm = new QFrame();
    carteForm->setObjectName("carte");

    QVBoxLayout *lf = new QVBoxLayout(carteForm);
    lf->setContentsMargins(22, 12, 22, 12);
    lf->setSpacing(8);

    QLabel *titreForm = new QLabel("👤  Informations de l'utilisateur");
    titreForm->setObjectName("titreCarte");

    erreur = new QLabel();

    QHBoxLayout *ligneTitre = new QHBoxLayout();
    ligneTitre->addWidget(titreForm);
    ligneTitre->addStretch();
    ligneTitre->addWidget(erreur);

    lf->addLayout(ligneTitre);

    champId = new QLineEdit();
    champNom = new QLineEdit();
    champPrenom = new QLineEdit();

    champTel = new QLineEdit();
    champTel->setMaxLength(8);
    champTel->setPlaceholderText("8 chiffres");

    champEmail = new QLineEdit();
    champEmail->setPlaceholderText("exemple@mail.com");

    champRole = new QComboBox();
    champRole->addItems(rolesDisponibles);

    champMdp = new QLineEdit();
    champMdp->setEchoMode(QLineEdit::Password);

    infoAcces = new QLabel();
    infoAcces->setObjectName("petit");

    auto etiquette = [](const QString &texte) -> QLabel *
    {
        QLabel *l = new QLabel(texte);
        l->setObjectName("labelChamp");
        return l;
    };

    QGridLayout *g = new QGridLayout();
    g->setHorizontalSpacing(14);
    g->setVerticalSpacing(8);

    g->addWidget(etiquette("ID Utilisateur :"), 0, 0);
    g->addWidget(champId, 0, 1);
    g->addWidget(etiquette("Nom :"), 0, 2);
    g->addWidget(champNom, 0, 3);

    g->addWidget(etiquette("Prénom :"), 1, 0);
    g->addWidget(champPrenom, 1, 1);
    g->addWidget(etiquette("Téléphone :"), 1, 2);
    g->addWidget(champTel, 1, 3);

    g->addWidget(etiquette("Email :"), 2, 0);
    g->addWidget(champEmail, 2, 1);
    g->addWidget(etiquette("Rôle :"), 2, 2);
    g->addWidget(champRole, 2, 3);

    g->addWidget(etiquette("Mot de passe :"), 3, 0);
    g->addWidget(champMdp, 3, 1);
    g->addWidget(etiquette("Accès :"), 3, 2);
    g->addWidget(infoAcces, 3, 3);

    g->setColumnMinimumWidth(0, 110);
    g->setColumnMinimumWidth(2, 110);
    g->setColumnStretch(1, 1);
    g->setColumnStretch(3, 1);

    lf->addLayout(g);

    lc->addWidget(carteForm);


    // ========================================================
    // BARRE DE BOUTONS
    // ========================================================

    QFrame *barre = new QFrame();
    barre->setObjectName("carte");

    QHBoxLayout *lb = new QHBoxLayout(barre);
    lb->setContentsMargins(12, 8, 12, 8);
    lb->setSpacing(8);

    btnAjouter = new QPushButton("＋  Ajouter");
    btnAjouter->setObjectName("btnPrimaire");

    btnModifier = new QPushButton("✎  Modifier");
    btnModifier->setObjectName("btnSecondaire");

    btnSupprimer = new QPushButton("🗑  Supprimer");
    btnSupprimer->setObjectName("btnDanger");

    btnVider = new QPushButton("↻  Vider");
    btnVider->setObjectName("btnSecondaire");

    QLabel *labelTri = new QLabel("⇅  Trier par");

    tri = new QComboBox();
    tri->addItem("ID");
    tri->addItem("Nom");
    tri->addItem("Prénom");
    tri->addItem("Rôle");
    tri->addItem("Email");
    tri->setMinimumWidth(110);

    btnTrier = new QPushButton("Trier");
    btnTrier->setObjectName("btnSecondaire");

    recherche = new QLineEdit();
    recherche->setPlaceholderText("Rechercher par ID ou nom...");
    recherche->setMinimumWidth(170);

    btnRechercher = new QPushButton("🔍");
    btnRechercher->setObjectName("btnSecondaire");

    btnExporter = new QPushButton("📄  Exporter en PDF");
    btnExporter->setObjectName("btnSecondaire");

    lb->addWidget(btnAjouter);
    lb->addWidget(btnModifier);
    lb->addWidget(btnSupprimer);
    lb->addWidget(btnVider);
    lb->addStretch();
    lb->addWidget(labelTri);
    lb->addWidget(tri);
    lb->addWidget(btnTrier);
    lb->addWidget(recherche);
    lb->addWidget(btnRechercher);
    lb->addWidget(btnExporter);

    lc->addWidget(barre);


    // ========================================================
    // LISTE (à gauche) + STATISTIQUES (à droite)
    // ========================================================

    QHBoxLayout *milieu = new QHBoxLayout();
    milieu->setSpacing(12);


    // ----- Liste -----
    QFrame *carteListe = new QFrame();
    carteListe->setObjectName("carte");

    QVBoxLayout *lt = new QVBoxLayout(carteListe);
    lt->setContentsMargins(14, 10, 14, 8);
    lt->setSpacing(6);

    QLabel *titreListe = new QLabel("📋  Liste des utilisateurs");
    titreListe->setObjectName("titreCarte");

    QLabel *aide = new QLabel("Sélectionnez une ligne pour modifier les informations");
    aide->setObjectName("petit");

    QHBoxLayout *enteteListe = new QHBoxLayout();
    enteteListe->addWidget(titreListe);
    enteteListe->addStretch();
    enteteListe->addWidget(aide);

    table = new QTableWidget();
    table->setColumnCount(7);
    table->setMinimumHeight(120);

    QStringList colonnes;
    colonnes << "ID" << "Nom" << "Prénom" << "Téléphone"
             << "Rôle" << "Email" << "Accès";
    table->setHorizontalHeaderLabels(colonnes);

    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setShowGrid(false);
    table->setFocusPolicy(Qt::NoFocus);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setDefaultSectionSize(36);

    labelNombre = new QLabel("0 utilisateur(s) trouvé(s)");
    labelNombre->setObjectName("petit");

    lt->addLayout(enteteListe);
    lt->addWidget(table);
    lt->addWidget(labelNombre);

    milieu->addWidget(carteListe, 1);


    // ----- Colonne de droite -----
    QVBoxLayout *colDroite = new QVBoxLayout();
    colDroite->setSpacing(10);

    // Carte statistiques (donut à gauche, légende à droite)
    QFrame *carteStats = new QFrame();
    carteStats->setObjectName("carte");
    carteStats->setFixedWidth(430);

    QVBoxLayout *ls = new QVBoxLayout(carteStats);
    ls->setContentsMargins(16, 12, 16, 12);
    ls->setSpacing(6);

    QLabel *titreStats = new QLabel("📊  Statistiques des utilisateurs");
    titreStats->setObjectName("titreCarte");

    donut = new DonutStats();
    donut->setFixedSize(170, 170);

    legendeStats = new QLabel();
    legendeStats->setObjectName("legende");
    legendeStats->setWordWrap(true);

    QHBoxLayout *lstats = new QHBoxLayout();
    lstats->setSpacing(12);
    lstats->addWidget(donut);
    lstats->addWidget(legendeStats, 1);

    ls->addWidget(titreStats);
    ls->addLayout(lstats);

    // Carte activité récente
    QFrame *carteActivite = new QFrame();
    carteActivite->setObjectName("carte");
    carteActivite->setFixedWidth(430);

    QVBoxLayout *la = new QVBoxLayout(carteActivite);
    la->setContentsMargins(16, 12, 16, 12);
    la->setSpacing(6);

    QLabel *titreActivite = new QLabel("🕒  Activité récente");
    titreActivite->setObjectName("titreCarte");

    labelActivite = new QLabel("Aucune activité pour le moment.");
    labelActivite->setObjectName("legende");
    labelActivite->setWordWrap(true);
    labelActivite->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    la->addWidget(titreActivite);
    la->addWidget(labelActivite);

    colDroite->addWidget(carteStats);
    colDroite->addWidget(carteActivite);
    colDroite->addStretch();

    milieu->addLayout(colDroite);

    lc->addLayout(milieu, 1);

    ld->addWidget(corps, 1);
    principal->addWidget(droite, 1);


    // ========================================================
    // CONNEXIONS
    // ========================================================

    connect(btnAjouter, SIGNAL(clicked()), this, SLOT(ajouterUtilisateur()));
    connect(btnModifier, SIGNAL(clicked()), this, SLOT(modifierUtilisateur()));
    connect(btnSupprimer, SIGNAL(clicked()), this, SLOT(supprimerUtilisateur()));
    connect(btnVider, SIGNAL(clicked()), this, SLOT(viderFormulaire()));
    connect(btnTrier, SIGNAL(clicked()), this, SLOT(trierUtilisateurs()));
    connect(btnRechercher, SIGNAL(clicked()), this, SLOT(rechercherUtilisateur()));
    connect(btnExporter, SIGNAL(clicked()), this, SLOT(exporterPDF()));

    connect(recherche, SIGNAL(textChanged(QString)), this, SLOT(rechercherUtilisateur()));
    connect(champRole, SIGNAL(currentIndexChanged(int)), this, SLOT(majAccesFormulaire()));
    connect(table, SIGNAL(itemSelectionChanged()), this, SLOT(chargerSelection()));


    // ========================================================
    // STYLE
    // ========================================================

    centralWidget->setStyleSheet(
        // Menu
        "QFrame#menu { background: #1B488C; }"
        "QLabel#logoSoleil { color: #FBBF24; font-size: 40px; }"
        "QLabel#logoTexte { color: white; font-size: 16px; font-weight: bold; }"
        "QFrame#menu QPushButton { color: white; background: transparent; border: none;"
        "text-align: left; padding: 12px 16px; font-size: 14px; border-radius: 6px; }"
        "QFrame#menu QPushButton:hover { background: #2A5FAE; }"
        "QFrame#menu QPushButton:checked { background: #2F7BD3; font-weight: bold; }"

        // En-tête
        "QFrame#entete { background: #2F7BD3; }"
        "QLabel#titrePage { color: white; font-size: 22px; font-weight: bold; }"
        "QLabel#sousTitre { color: #DCEBFF; font-size: 12px; }"
        "QLabel#enteteInfo { color: white; font-size: 13px; font-weight: bold; }"
        "QPushButton#btnDeconnexion { background: rgba(255,255,255,40); color: white;"
        "border: 1px solid rgba(255,255,255,140); border-radius: 6px; padding: 7px 14px; }"
        "QPushButton#btnDeconnexion:hover { background: rgba(255,255,255,80); }"

        // Corps et cartes
        "QFrame#corps { background: #F1F5FB; }"
        "QFrame#carte { background: white; border: 1px solid #E5EAF3; border-radius: 10px; }"
        "QLabel#titreCarte { font-size: 15px; font-weight: bold; color: #0F2A52; }"
        "QLabel#texteCarte { font-size: 13px; color: #334155; }"
        "QLabel#petit { font-size: 12px; color: #64748B; }"
        "QLabel#legende { font-size: 12px; color: #334155; }"
        "QLabel#labelChamp { font-size: 13px; color: #334155; }"

        // Boutons
        "QPushButton#btnPrimaire { background: #2563EB; color: white; border: none;"
        "border-radius: 6px; padding: 6px 14px; font-size: 13px; }"
        "QPushButton#btnPrimaire:hover { background: #1D4ED8; }"
        "QPushButton#btnSecondaire { background: white; color: #1E293B;"
        "border: 1px solid #CBD5E1; border-radius: 6px; padding: 6px 12px; font-size: 13px; }"
        "QPushButton#btnSecondaire:hover { background: #F1F5F9; }"
        "QPushButton#btnDanger { background: white; color: #DC2626;"
        "border: 1px solid #CBD5E1; border-radius: 6px; padding: 6px 12px; font-size: 13px; }"
        "QPushButton#btnDanger:hover { background: #FEF2F2; }"

        // Champs
        "QLineEdit, QComboBox { border: 1px solid #CBD5E1; border-radius: 5px;"
        "padding: 4px 8px; background: white; color: #1E293B; min-height: 20px; }"
        "QLineEdit:disabled { background: #F1F5F9; color: #94A3B8; }"

        // Tableau
        "QTableWidget { border: 1px solid #E2E8F0; background: white; font-size: 13px; }"
        "QTableWidget::item { border-bottom: 1px solid #EEF2F7; padding: 4px; }"
        "QTableWidget::item:selected { background: #DBEAFE; color: #0F2A52; }"
        "QHeaderView::section { background: #F1F5F9; color: #0F2A52;"
        "font-weight: bold; padding: 6px; border: none; border-right: 1px solid #E2E8F0; }"
        "QTableCornerButton::section { background: #F1F5F9; border: none; }"
        );
}


// ============================================================
// ACCUEIL : CONNEXION + MOT DE PASSE OUBLIE
// ============================================================

void Gutilisateur::construireLogin()
{
    pageLogin = new QFrame();
    pageLogin->setObjectName("pageLogin");

    QVBoxLayout *lp = new QVBoxLayout(pageLogin);
    lp->setAlignment(Qt::AlignCenter);

    QFrame *carte = new QFrame();
    carte->setObjectName("carteLogin");
    carte->setFixedWidth(440);

    QVBoxLayout *lcarte = new QVBoxLayout(carte);
    lcarte->setContentsMargins(35, 30, 35, 30);
    lcarte->setSpacing(12);

    QLabel *logo = new QLabel("☁  Gestion des données météorologiques");
    logo->setObjectName("logoLogin");
    logo->setAlignment(Qt::AlignCenter);
    logo->setWordWrap(true);

    pileLogin = new QStackedWidget();

    lcarte->addWidget(logo);
    lcarte->addWidget(pileLogin);
    lp->addWidget(carte);

    auto titreVue = [](const QString &t) -> QLabel *
    {
        QLabel *l = new QLabel(t);
        l->setObjectName("titreLogin");
        return l;
    };

    auto sousVue = [](const QString &t) -> QLabel *
    {
        QLabel *l = new QLabel(t);
        l->setObjectName("sousLogin");
        l->setWordWrap(true);
        return l;
    };

    auto lien = [](const QString &t) -> QPushButton *
    {
        QPushButton *b = new QPushButton(t);
        b->setObjectName("lien");
        b->setCursor(Qt::PointingHandCursor);
        return b;
    };


    // ---------- Vue 0 : connexion ----------
    QWidget *vue0 = new QWidget();
    QVBoxLayout *l0 = new QVBoxLayout(vue0);
    l0->setContentsMargins(0, 8, 0, 0);
    l0->setSpacing(10);

    loginEmail = new QLineEdit();
    loginEmail->setPlaceholderText("✉  Email");

    loginMdp = new QLineEdit();
    loginMdp->setEchoMode(QLineEdit::Password);
    loginMdp->setPlaceholderText("🔒  Mot de passe");

    QCheckBox *afficherMdp = new QCheckBox("Afficher le mot de passe");

    loginErreur = new QLabel();
    loginErreur->setWordWrap(true);

    btnConnexion = new QPushButton("Se connecter");
    btnConnexion->setObjectName("btnLogin");

    QPushButton *btnOublie = lien("Mot de passe oublié ?");

    l0->addWidget(titreVue("Connexion"));
    l0->addWidget(sousVue("Accès réservé aux administrateurs et aux responsables RH"));
    l0->addSpacing(6);
    l0->addWidget(loginEmail);
    l0->addWidget(loginMdp);
    l0->addWidget(afficherMdp);
    l0->addWidget(loginErreur);
    l0->addWidget(btnConnexion);
    l0->addWidget(btnOublie, 0, Qt::AlignCenter);

    pileLogin->addWidget(vue0);


    // ---------- Vue 1 : mot de passe oublié (envoi du code) ----------
    QWidget *vue1 = new QWidget();
    QVBoxLayout *l1 = new QVBoxLayout(vue1);
    l1->setContentsMargins(0, 8, 0, 0);
    l1->setSpacing(10);

    radioEmail = new QRadioButton("Recevoir le code par email");
    radioTel = new QRadioButton("Recevoir le code par SMS (numéro de téléphone)");
    radioEmail->setChecked(true);

    recupContact = new QLineEdit();
    recupContact->setPlaceholderText("✉  Votre adresse email");

    recupInfo1 = new QLabel();
    recupInfo1->setWordWrap(true);

    QPushButton *btnEnvoyer = new QPushButton("Envoyer le code");
    btnEnvoyer->setObjectName("btnLogin");

    QPushButton *retour1 = lien("←  Retour à la connexion");

    l1->addWidget(titreVue("Mot de passe oublié"));
    l1->addWidget(sousVue("Choisissez comment recevoir votre code de vérification"));
    l1->addSpacing(6);
    l1->addWidget(radioEmail);
    l1->addWidget(radioTel);
    l1->addWidget(recupContact);
    l1->addWidget(recupInfo1);
    l1->addWidget(btnEnvoyer);
    l1->addWidget(retour1, 0, Qt::AlignCenter);

    pileLogin->addWidget(vue1);


    // ---------- Vue 2 : code + nouveau mot de passe ----------
    QWidget *vue2 = new QWidget();
    QVBoxLayout *l2 = new QVBoxLayout(vue2);
    l2->setContentsMargins(0, 8, 0, 0);
    l2->setSpacing(10);

    recupCode = new QLineEdit();
    recupCode->setMaxLength(6);
    recupCode->setPlaceholderText("🔑  Code à 6 chiffres");

    recupNouveau = new QLineEdit();
    recupNouveau->setEchoMode(QLineEdit::Password);
    recupNouveau->setPlaceholderText("🔒  Nouveau mot de passe");

    recupConfirm = new QLineEdit();
    recupConfirm->setEchoMode(QLineEdit::Password);
    recupConfirm->setPlaceholderText("🔒  Confirmer le mot de passe");

    recupInfo2 = new QLabel();
    recupInfo2->setWordWrap(true);

    QPushButton *btnReset = new QPushButton("Réinitialiser le mot de passe");
    btnReset->setObjectName("btnLogin");

    QPushButton *retour2 = lien("←  Retour à la connexion");

    l2->addWidget(titreVue("Vérification"));
    l2->addWidget(sousVue("Saisissez le code reçu, puis choisissez un nouveau mot de passe"));
    l2->addSpacing(6);
    l2->addWidget(recupCode);
    l2->addWidget(recupNouveau);
    l2->addWidget(recupConfirm);
    l2->addWidget(recupInfo2);
    l2->addWidget(btnReset);
    l2->addWidget(retour2, 0, Qt::AlignCenter);

    pileLogin->addWidget(vue2);


    // ---------- Connexions ----------
    connect(btnConnexion, SIGNAL(clicked()), this, SLOT(seConnecter()));
    connect(loginEmail, SIGNAL(returnPressed()), this, SLOT(seConnecter()));
    connect(loginMdp, SIGNAL(returnPressed()), this, SLOT(seConnecter()));
    connect(btnOublie, SIGNAL(clicked()), this, SLOT(ouvrirRecuperation()));
    connect(retour1, SIGNAL(clicked()), this, SLOT(retourConnexion()));
    connect(retour2, SIGNAL(clicked()), this, SLOT(retourConnexion()));
    connect(radioEmail, SIGNAL(toggled(bool)), this, SLOT(majPlaceholderRecup()));
    connect(btnEnvoyer, SIGNAL(clicked()), this, SLOT(envoyerCode()));
    connect(recupContact, SIGNAL(returnPressed()), this, SLOT(envoyerCode()));
    connect(btnReset, SIGNAL(clicked()), this, SLOT(reinitialiserMdp()));

    connect(afficherMdp, &QCheckBox::toggled, [this](bool on)
            {
                loginMdp->setEchoMode(on ? QLineEdit::Normal : QLineEdit::Password);
            });


    // ---------- Style ----------
    pageLogin->setStyleSheet(
        "QFrame#pageLogin { background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
        "stop:0 #0B3A8F, stop:1 #4A9BE8); }"
        "QFrame#carteLogin { background: white; border-radius: 14px; }"
        "QLabel#logoLogin { color: #1E55C8; font-size: 15px; font-weight: bold; }"
        "QLabel#titreLogin { color: #1E293B; font-size: 24px; font-weight: bold; }"
        "QLabel#sousLogin { color: #64748B; font-size: 13px; }"
        "QLineEdit { border: 1px solid #CBD5E1; border-radius: 8px; padding: 11px;"
        "background: white; color: #1E293B; font-size: 14px; }"
        "QLineEdit:focus { border: 1px solid #2563EB; }"
        "QRadioButton, QCheckBox { color: #1E293B; font-size: 13px; }"
        "QPushButton#btnLogin { background: #2563EB; color: white; border: none;"
        "border-radius: 8px; padding: 12px; font-weight: bold; font-size: 14px; }"
        "QPushButton#btnLogin:hover { background: #1D4ED8; }"
        "QPushButton#btnLogin:disabled { background: #93C5FD; }"
        "QPushButton#lien { background: transparent; border: none;"
        "color: #2563EB; font-size: 13px; padding: 4px; }"
        "QPushButton#lien:hover { text-decoration: underline; }"
        );

    pile->addWidget(pageLogin);   // page 0
}

void Gutilisateur::seConnecter()
{
    if (!btnConnexion->isEnabled())
        return;

    QString email = loginEmail->text().trimmed();
    QString mdp = loginMdp->text();

    loginErreur->setStyleSheet("color: #DC2626; font-weight: bold;");

    if (email.isEmpty() || mdp.isEmpty())
    {
        loginErreur->setText("⚠  Saisissez votre email et votre mot de passe.");
        return;
    }

    for (int i = 0; i < utilisateurs.size(); i++)
    {
        const Utilisateur &u = utilisateurs.at(i);

        if (aAcces(u.role)
            && !u.mdp.isEmpty()
            && u.email.compare(email, Qt::CaseInsensitive) == 0
            && u.mdp == hacher(mdp))
        {
            utilisateurConnecte = u.id;
            loginEchecs = 0;

            labelConnecte->setText("👤  " + u.prenom + " " + u.nom + "  ·  " + u.role);

            enregistrerAction("Connexion : " + u.email);

            loginEmail->clear();
            loginMdp->clear();
            loginErreur->clear();

            pile->setCurrentIndex(1);
            return;
        }
    }

    loginEchecs++;
    loginMdp->clear();

    if (loginEchecs >= 5)
    {
        btnConnexion->setEnabled(false);
        loginErreur->setText("⚠  Trop de tentatives. Réessayez dans 30 secondes.");

        QTimer::singleShot(30000, this, [this]()
                           {
                               btnConnexion->setEnabled(true);
                               loginEchecs = 0;
                               loginErreur->clear();
                           });

        return;
    }

    loginErreur->setText("⚠  Email ou mot de passe incorrect.");
}

void Gutilisateur::deconnecter()
{
    enregistrerAction("Déconnexion : ID " + QString::number(utilisateurConnecte));

    utilisateurConnecte = -1;
    loginEchecs = 0;

    recherche->clear();
    viderFormulaire();
    retourConnexion();

    pile->setCurrentIndex(0);
}

void Gutilisateur::ouvrirRecuperation()
{
    recupContact->clear();
    recupInfo1->clear();
    radioEmail->setChecked(true);
    majPlaceholderRecup();

    pileLogin->setCurrentIndex(1);
}

void Gutilisateur::retourConnexion()
{
    recupContact->clear();
    recupCode->clear();
    recupNouveau->clear();
    recupConfirm->clear();
    recupInfo1->clear();
    recupInfo2->clear();

    recupCodeAttendu.clear();
    recupId = -1;

    loginMdp->clear();
    loginErreur->clear();

    pileLogin->setCurrentIndex(0);
}

void Gutilisateur::majPlaceholderRecup()
{
    recupContact->setPlaceholderText(radioEmail->isChecked()
                                     ? "✉  Votre adresse email"
                                     : "📱  Votre numéro de téléphone (8 chiffres)");

    recupContact->clear();
    recupInfo1->clear();
}

void Gutilisateur::envoyerCode()
{
    QString contact = recupContact->text().trimmed();
    bool parEmail = radioEmail->isChecked();

    recupInfo1->setStyleSheet("color: #DC2626; font-weight: bold;");

    if (contact.isEmpty())
    {
        recupInfo1->setText(parEmail ? "⚠  Saisissez votre adresse email."
                                     : "⚠  Saisissez votre numéro de téléphone.");
        return;
    }

    // On cherche un compte AVEC accès qui correspond
    int index = -1;

    for (int i = 0; i < utilisateurs.size(); i++)
    {
        const Utilisateur &u = utilisateurs.at(i);

        bool correspond = parEmail
                              ? (u.email.compare(contact, Qt::CaseInsensitive) == 0)
                              : (u.tel == contact);

        if (correspond && aAcces(u.role))
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        recupInfo1->setText("⚠  Aucun compte avec accès ne correspond à ces informations.");
        return;
    }

    // Code à 6 chiffres
    int code;

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    code = QRandomGenerator::global()->bounded(100000, 1000000);
#else
    qsrand(QTime::currentTime().msecsSinceStartOfDay());
    code = 100000 + qrand() % 900000;
#endif

    recupId = utilisateurs.at(index).id;
    recupCodeAttendu = QString::number(code);
    recupExpiration = QDateTime::currentDateTime().addSecs(300);   // 5 minutes
    recupEssais = 0;

    QString canal = parEmail ? "par email à " + contact
                             : "par SMS au ***" + contact.right(3);

    // SIMULATION : ici on affiche le code au lieu de l'envoyer vraiment
    QMessageBox::information(this, "Simulation d'envoi",
                             "Un code de vérification a été envoyé " + canal + " :\n\n"
                                 + recupCodeAttendu + "\n\nValable 5 minutes.\n\n"
                                                      "(Simulation : un vrai envoi d'email ou de SMS nécessite un serveur SMTP "
                                                      "ou une API SMS.)");

    recupCode->clear();
    recupNouveau->clear();
    recupConfirm->clear();
    recupInfo2->clear();

    pileLogin->setCurrentIndex(2);
}

void Gutilisateur::reinitialiserMdp()
{
    recupInfo2->setStyleSheet("color: #DC2626; font-weight: bold;");

    int index = trouverUtilisateur(recupId);

    if (index == -1 || recupCodeAttendu.isEmpty())
    {
        retourConnexion();
        return;
    }

    if (QDateTime::currentDateTime() > recupExpiration)
    {
        recupCodeAttendu.clear();
        pileLogin->setCurrentIndex(1);
        recupInfo1->setStyleSheet("color: #DC2626; font-weight: bold;");
        recupInfo1->setText("⚠  Le code a expiré. Demandez-en un nouveau.");
        return;
    }

    if (recupCode->text().trimmed() != recupCodeAttendu)
    {
        recupEssais++;

        if (recupEssais >= 3)
        {
            recupCodeAttendu.clear();
            pileLogin->setCurrentIndex(1);
            recupInfo1->setStyleSheet("color: #DC2626; font-weight: bold;");
            recupInfo1->setText("⚠  Trop d'essais. Demandez un nouveau code.");
            return;
        }

        recupInfo2->setText(QString("⚠  Code incorrect (%1/3).").arg(recupEssais));
        return;
    }

    QString nouveau = recupNouveau->text();

    if (nouveau.length() < 4)
    {
        recupInfo2->setText("⚠  Le mot de passe doit contenir au moins 4 caractères.");
        return;
    }

    if (nouveau != recupConfirm->text())
    {
        recupInfo2->setText("⚠  Les deux mots de passe ne sont pas identiques.");
        return;
    }

    utilisateurs[index].mdp = hacher(nouveau);

    enregistrerAction("Réinitialisation du mot de passe, utilisateur ID "
                      + QString::number(recupId));

    retourConnexion();

    loginErreur->setStyleSheet("color: #16A34A; font-weight: bold;");
    loginErreur->setText("✔  Mot de passe réinitialisé. Vous pouvez vous connecter.");
}

QString Gutilisateur::hacher(const QString &mdp)
{
    return QString(QCryptographicHash::hash(mdp.toUtf8(),
                                            QCryptographicHash::Sha256).toHex());
}


// ============================================================
// DONNEES TEST
// ============================================================

void Gutilisateur::ajouterDonneesTest()
{
    Utilisateur u1;
    u1.id = 1; u1.nom = "Ben Ali"; u1.prenom = "Ahmed";
    u1.tel = "22111222"; u1.role = "Administrateur";
    u1.mdp = hacher("1234"); u1.email = "ahmed@gmail.com";
    utilisateurs.append(u1);

    Utilisateur u2;
    u2.id = 2; u2.nom = "Trabelsi"; u2.prenom = "Sara";
    u2.tel = "22333444"; u2.role = "Responsable RH";
    u2.mdp = hacher("5678"); u2.email = "sara@gmail.com";
    utilisateurs.append(u2);

    Utilisateur u3;
    u3.id = 3; u3.nom = "Mansouri"; u3.prenom = "Amine";
    u3.tel = "22555666"; u3.role = "Responsable RH";
    u3.mdp = hacher("abcd"); u3.email = "amine@gmail.com";
    utilisateurs.append(u3);

    Utilisateur u4;
    u4.id = 4; u4.nom = "Gharbi"; u4.prenom = "Youssef";
    u4.tel = "22777888"; u4.role = "Technicien";
    u4.mdp = ""; u4.email = "youssef@gmail.com";
    utilisateurs.append(u4);

    Utilisateur u5;
    u5.id = 5; u5.nom = "Jlassi"; u5.prenom = "Mariem";
    u5.tel = "22999000"; u5.role = "Météorologue";
    u5.mdp = ""; u5.email = "mariem@gmail.com";
    utilisateurs.append(u5);

    Utilisateur u6;
    u6.id = 6; u6.nom = "Bouzid"; u6.prenom = "Salma";
    u6.tel = "22123456"; u6.role = "Responsable de station";
    u6.mdp = ""; u6.email = "salma@gmail.com";
    utilisateurs.append(u6);
}


// ============================================================
// TABLEAU
// ============================================================

void Gutilisateur::remplirTableau()
{
    table->setRowCount(0);

    for (int i = 0; i < utilisateurs.size(); i++)
    {
        const Utilisateur &u = utilisateurs.at(i);

        table->insertRow(i);

        table->setItem(i, 0, new QTableWidgetItem(QString::number(u.id)));
        table->setItem(i, 1, new QTableWidgetItem(u.nom));
        table->setItem(i, 2, new QTableWidgetItem(u.prenom));
        table->setItem(i, 3, new QTableWidgetItem(u.tel));
        table->setItem(i, 4, new QTableWidgetItem(u.role));
        table->setItem(i, 5, new QTableWidgetItem(u.email));

        QTableWidgetItem *itemAcces =
            new QTableWidgetItem(aAcces(u.role) ? "Oui" : "Non");
        itemAcces->setForeground(QColor(aAcces(u.role) ? 0x16A34A : 0x94A3B8));
        table->setItem(i, 6, itemAcces);
    }

    // Garde la ligne en cours de modification sélectionnée (sans recharger le formulaire)
    if (idEnEdition != -1)
    {
        int index = trouverUtilisateur(idEnEdition);

        if (index != -1)
        {
            table->blockSignals(true);
            table->selectRow(index);
            table->blockSignals(false);
        }
    }

    rechercherUtilisateur();
}


// ============================================================
// STATISTIQUES
// ============================================================

void Gutilisateur::mettreAJourStatistiques()
{
    int total = utilisateurs.size();
    int avecAcces = 0;

    QList<int> valeurs;
    QString legende;

    for (int r = 0; r < rolesDisponibles.size(); r++)
    {
        int n = 0;

        for (int i = 0; i < utilisateurs.size(); i++)
        {
            if (utilisateurs.at(i).role == rolesDisponibles.at(r))
                n++;
        }

        if (n == 0)
            continue;

        int pct = (total > 0) ? n * 100 / total : 0;
        QString couleur = DonutStats::couleur(valeurs.size()).name();

        legende += QString("<span style='color:%1;'>●</span>&nbsp; %2 &nbsp;<b>%3</b> (%4%)<br>")
                       .arg(couleur)
                       .arg(rolesDisponibles.at(r))
                       .arg(n)
                       .arg(pct);

        valeurs.append(n);

        if (aAcces(rolesDisponibles.at(r)))
            avecAcces += n;
    }

    legende += QString("<br>🔑 Avec accès : <b>%1</b> &nbsp;|&nbsp; Sans accès : <b>%2</b>")
                   .arg(avecAcces)
                   .arg(total - avecAcces);

    donut->setValeurs(valeurs);
    legendeStats->setText(legende);
}


// ============================================================
// FORMULAIRE
// ============================================================

void Gutilisateur::afficherMessage(const QString &texte, bool estErreur)
{
    if (estErreur)
        erreur->setStyleSheet("color: #DC2626; font-weight: bold;");
    else
        erreur->setStyleSheet("color: #16A34A; font-weight: bold;");

    erreur->setText(texte);
}

void Gutilisateur::majAccesFormulaire()
{
    bool acces = aAcces(champRole->currentText());

    champMdp->setEnabled(acces);

    if (acces)
    {
        infoAcces->setText("🔑 Ce rôle a accès à l'application");
    }
    else
    {
        infoAcces->setText("Ce rôle n'a pas accès à l'application");
        champMdp->clear();
    }
}

void Gutilisateur::viderFormulaire()
{
    idEnEdition = -1;

    // ID proposé : le plus grand ID existant + 1
    int prochain = 1;
    for (int i = 0; i < utilisateurs.size(); i++)
    {
        if (utilisateurs.at(i).id >= prochain)
            prochain = utilisateurs.at(i).id + 1;
    }

    champId->clear();
    champId->setEnabled(true);
    champId->setPlaceholderText("Généré automatiquement si vide (ex. "
                                + QString::number(prochain) + ")");

    champNom->clear();
    champPrenom->clear();
    champTel->clear();
    champEmail->clear();
    champMdp->clear();
    champMdp->setPlaceholderText("Obligatoire pour les rôles avec accès");
    champRole->setCurrentIndex(0);

    erreur->clear();

    majAccesFormulaire();
    table->clearSelection();
}

void Gutilisateur::chargerPourModification(int id)
{
    int index = trouverUtilisateur(id);
    if (index == -1)
        return;

    const Utilisateur &u = utilisateurs.at(index);

    idEnEdition = id;

    champId->setText(QString::number(u.id));
    champId->setEnabled(false);

    champNom->setText(u.nom);
    champPrenom->setText(u.prenom);
    champTel->setText(u.tel);
    champEmail->setText(u.email);
    champRole->setCurrentIndex(qMax(0, rolesDisponibles.indexOf(u.role)));

    champMdp->clear();
    champMdp->setPlaceholderText(u.mdp.isEmpty() ? "Obligatoire pour les rôles avec accès"
                                                 : "Laisser vide pour conserver");
    erreur->clear();

    majAccesFormulaire();
}

void Gutilisateur::chargerSelection()
{
    int id = idSelectionne();

    if (id != -1)
        chargerPourModification(id);
}

// Vérifie le formulaire et remplit u. Retourne false (et affiche l'erreur) si invalide.
bool Gutilisateur::lireFormulaire(Utilisateur &u, bool edition)
{
    QString idTexte = champId->text().trimmed();
    QString nom = champNom->text().trimmed();
    QString prenom = champPrenom->text().trimmed();
    QString tel = champTel->text().trimmed();
    QString email = champEmail->text().trimmed();
    QString role = champRole->currentText();
    QString mdp = champMdp->text();

    bool acces = aAcces(role);
    bool garderMdp = edition && !u.mdp.isEmpty();

    int id = idEnEdition;

    if (!edition)
    {
        if (idTexte.isEmpty())
        {
            id = 1;
            for (int i = 0; i < utilisateurs.size(); i++)
            {
                if (utilisateurs.at(i).id >= id)
                    id = utilisateurs.at(i).id + 1;
            }
        }
        else
        {
            bool ok = false;
            id = idTexte.toInt(&ok);

            if (!ok || id < 1)
            {
                afficherMessage("⚠  L'ID doit être un nombre positif.", true);
                return false;
            }
        }

        if (trouverUtilisateur(id) != -1)
        {
            afficherMessage("⚠  Cet ID existe déjà. Utilisez « Modifier » ou « Vider ».", true);
            return false;
        }
    }

    if (nom.isEmpty() || prenom.isEmpty())
    {
        afficherMessage("⚠  Le nom et le prénom sont obligatoires.", true);
        return false;
    }

    bool telOk = (tel.length() == 8);
    for (int i = 0; i < tel.length(); i++)
    {
        if (!tel.at(i).isDigit())
            telOk = false;
    }

    if (!telOk)
    {
        afficherMessage("⚠  Le téléphone doit contenir exactement 8 chiffres.", true);
        return false;
    }

    int arobase = email.indexOf('@');
    bool emailOk = arobase > 0
                   && email.indexOf('.', arobase) > arobase + 1
                   && !email.endsWith('.');

    if (!emailOk)
    {
        afficherMessage("⚠  L'adresse email n'est pas valide.", true);
        return false;
    }

    if (acces && mdp.isEmpty() && !garderMdp)
    {
        afficherMessage("⚠  Un mot de passe est obligatoire pour ce rôle.", true);
        return false;
    }

    if (acces && !mdp.isEmpty() && mdp.length() < 4)
    {
        afficherMessage("⚠  Le mot de passe doit contenir au moins 4 caractères.", true);
        return false;
    }

    u.id = id;
    u.nom = nom;
    u.prenom = prenom;
    u.tel = tel;
    u.email = email;
    u.role = role;

    if (!acces)
        u.mdp.clear();
    else if (!mdp.isEmpty())
        u.mdp = hacher(mdp);

    return true;
}


// ============================================================
// AJOUTER / MODIFIER / SUPPRIMER
// ============================================================

void Gutilisateur::ajouterUtilisateur()
{
    Utilisateur u;

    if (!lireFormulaire(u, false))
        return;

    utilisateurs.append(u);

    enregistrerAction("Ajout utilisateur ID " + QString::number(u.id));

    trierUtilisateurs();
    mettreAJourStatistiques();
    viderFormulaire();

    afficherMessage("✔  Utilisateur ajouté avec succès.", false);
}

void Gutilisateur::modifierUtilisateur()
{
    if (idEnEdition == -1)
    {
        QMessageBox::information(this, "Information",
                                 "Sélectionnez d'abord une ligne dans la liste.");
        return;
    }

    int index = trouverUtilisateur(idEnEdition);

    if (index == -1)
    {
        viderFormulaire();
        return;
    }

    Utilisateur u = utilisateurs.at(index);

    if (!lireFormulaire(u, true))
        return;

    utilisateurs[index] = u;

    enregistrerAction("Modification utilisateur ID " + QString::number(u.id));

    trierUtilisateurs();
    mettreAJourStatistiques();
    viderFormulaire();

    afficherMessage("✔  Utilisateur modifié avec succès.", false);
}

void Gutilisateur::supprimerUtilisateur()
{
    int id = idSelectionne();

    if (id == -1)
    {
        QMessageBox::information(this, "Information",
                                 "Sélectionnez d'abord une ligne dans la liste.");
        return;
    }

    if (id == utilisateurConnecte)
    {
        QMessageBox::warning(this, "Suppression impossible",
                             "Vous ne pouvez pas supprimer le compte avec lequel vous êtes connecté(e).");
        return;
    }

    QMessageBox::StandardButton rep = QMessageBox::question(
        this, "Supprimer",
        "Voulez-vous supprimer cet utilisateur ?",
        QMessageBox::Yes | QMessageBox::No);

    if (rep != QMessageBox::Yes)
        return;

    int index = trouverUtilisateur(id);

    if (index != -1)
    {
        utilisateurs.removeAt(index);

        enregistrerAction("Suppression utilisateur ID " + QString::number(id));

        viderFormulaire();
        remplirTableau();
        mettreAJourStatistiques();

        afficherMessage("✔  Utilisateur supprimé.", false);
    }
}


// ============================================================
// TRI / RECHERCHE
// ============================================================

void Gutilisateur::trierUtilisateurs()
{
    int c = tri->currentIndex();

    std::sort(utilisateurs.begin(), utilisateurs.end(),
              [c](const Utilisateur &a, const Utilisateur &b) -> bool
              {
                  switch (c)
                  {
                  case 1:  return a.nom.toLower() < b.nom.toLower();
                  case 2:  return a.prenom.toLower() < b.prenom.toLower();
                  case 3:  return a.role.toLower() < b.role.toLower();
                  case 4:  return a.email.toLower() < b.email.toLower();
                  default: return a.id < b.id;
                  }
              });

    remplirTableau();
}

void Gutilisateur::rechercherUtilisateur()
{
    QString texte = recherche->text().trimmed().toLower();
    int visibles = 0;

    for (int i = 0; i < table->rowCount() && i < utilisateurs.size(); i++)
    {
        const Utilisateur &u = utilisateurs.at(i);

        bool trouve = texte.isEmpty()
                      || QString::number(u.id).contains(texte)
                      || u.nom.toLower().contains(texte)
                      || u.prenom.toLower().contains(texte)
                      || u.role.toLower().contains(texte)
                      || u.email.toLower().contains(texte);

        table->setRowHidden(i, !trouve);

        if (trouve)
            visibles++;
    }

    labelNombre->setText(QString::number(visibles) + " utilisateur(s) trouvé(s)");
}

void Gutilisateur::afficherTous()
{
    recherche->clear();
    remplirTableau();
}


// ============================================================
// OUTILS
// ============================================================

int Gutilisateur::idSelectionne()
{
    QList<QModelIndex> lignes = table->selectionModel()->selectedRows();

    if (lignes.isEmpty())
        return -1;

    int ligne = lignes.first().row();

    if (ligne < 0 || ligne >= utilisateurs.size())
        return -1;

    return utilisateurs.at(ligne).id;
}

int Gutilisateur::trouverUtilisateur(int id)
{
    for (int i = 0; i < utilisateurs.size(); i++)
    {
        if (utilisateurs.at(i).id == id)
            return i;
    }

    return -1;
}

// Seuls l'administrateur et le responsable RH ont accès à l'application
bool Gutilisateur::aAcces(const QString &role)
{
    return role == "Administrateur" || role == "Responsable RH";
}

void Gutilisateur::majHorloge()
{
    QDateTime maintenant = QDateTime::currentDateTime();

    labelDate->setText(maintenant.toString("dd/MM/yyyy"));
    labelHeure->setText(maintenant.toString("HH:mm"));
}

void Gutilisateur::enregistrerAction(const QString &action)
{
    // Journal dans un fichier
    QFile fichier("journal_actions.txt");

    if (fichier.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream flux(&fichier);

        flux << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss")
             << " - " << action << "\n";

        fichier.close();
    }

    // Activité récente (affichée à droite)
    activites.prepend(QDateTime::currentDateTime().toString("HH:mm")
                      + "  —  " + action);

    while (activites.size() > 4)
        activites.removeLast();

    if (labelActivite)
    {
        QString texte;

        for (int i = 0; i < activites.size(); i++)
            texte += "• " + activites.at(i).toHtmlEscaped() + "<br>";

        labelActivite->setText(texte);
    }
}


// ============================================================
// EXPORT PDF
// ============================================================

void Gutilisateur::exporterPDF()
{
    QString chemin = QDir::homePath() + "/utilisateurs.pdf";

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(chemin);

    QTextDocument document;

    QString html;

    html += "<html><head><style>"
            "table { border-collapse: collapse; width: 100%; }"
            "th, td { border: 1px solid black; padding: 6px; }"
            "th { background-color: #eeeeee; }"
            "</style></head><body>";

    html += "<h1>Liste des utilisateurs</h1>";
    html += "<p>Date : " +
            QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm") +
            "</p>";

    html += "<table><tr>"
            "<th>ID</th><th>Nom</th><th>Prénom</th>"
            "<th>Téléphone</th><th>Rôle</th><th>Accès</th><th>Email</th>"
            "</tr>";

    for (int i = 0; i < utilisateurs.size(); i++)
    {
        const Utilisateur &u = utilisateurs.at(i);

        html += "<tr>";
        html += "<td>" + QString::number(u.id) + "</td>";
        html += "<td>" + u.nom + "</td>";
        html += "<td>" + u.prenom + "</td>";
        html += "<td>" + u.tel + "</td>";
        html += "<td>" + u.role + "</td>";
        html += QString("<td>") + (aAcces(u.role) ? "Oui" : "Non") + "</td>";
        html += "<td>" + u.email + "</td>";
        html += "</tr>";
    }

    html += "</table></body></html>";

    document.setHtml(html);
    document.print(&printer);

    QMessageBox::information(this, "Export PDF",
                             "Le fichier PDF a été créé dans :\n\n" + chemin);

    enregistrerAction("Export PDF des utilisateurs");
}