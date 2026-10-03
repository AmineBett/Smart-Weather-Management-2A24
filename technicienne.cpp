#include "technicienne.h"

// =====================================================
// STATISTICS WIDGET
// =====================================================

StatisticsWidget::StatisticsWidget(QWidget *parent)
    : QWidget(parent),
    actifs(0),
    inactifs(0)
{
    setMinimumSize(260, 220);
    setMaximumHeight(245);
}

void StatisticsWidget::setStatistics(int actifsValue, int inactifsValue)
{
    actifs = actifsValue;
    inactifs = inactifsValue;

    update();
}

void StatisticsWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    int total = actifs + inactifs;

    if (total == 0)
        total = 1;

    // Centre du cercle
    QPoint center(width() / 2, 115);

    // Taille du cercle
    int diameter = 130;

    QRectF rect(
        center.x() - diameter / 2,
        center.y() - diameter / 2,
        diameter,
        diameter
        );

    // =========================
    // CERCLE GRIS DE BASE
    // =========================

    QPen backgroundPen(QColor("#E8EEF5"));
    backgroundPen.setWidth(22);

    painter.setPen(backgroundPen);
    painter.drawArc(rect, 0, 360 * 16);


    // =========================
    // PARTIE ACTIVE
    // =========================

    double angleActifs =
        360.0 * actifs / total;

    QPen activePen(QColor("#18A66A"));
    activePen.setWidth(22);
    activePen.setCapStyle(Qt::FlatCap);

    painter.setPen(activePen);

    painter.drawArc(
        rect,
        90 * 16,
        -angleActifs * 16
        );


    // =========================
    // PARTIE INACTIVE
    // =========================

    double angleInactifs =
        360.0 * inactifs / total;

    QPen inactivePen(QColor("#EF5350"));
    inactivePen.setWidth(22);
    inactivePen.setCapStyle(Qt::FlatCap);

    painter.setPen(inactivePen);

    painter.drawArc(
        rect,
        (90 - angleActifs) * 16,
        -angleInactifs * 16
        );


    // =========================
    // CENTRE BLANC
    // =========================

    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);

    painter.drawEllipse(
        center,
        42,
        42
        );


    // =========================
    // TOTAL
    // =========================

    painter.setPen(QColor("#173F73"));

    QFont fontTotal;
    fontTotal.setPointSize(19);
    fontTotal.setBold(true);

    painter.setFont(fontTotal);

    QRect totalRect(
        center.x() - 45,
        center.y() - 17,
        90,
        30
        );

    painter.drawText(
        totalRect,
        Qt::AlignCenter,
        QString::number(actifs + inactifs)
        );


    // =========================
    // TEXTE TOTAL
    // =========================

    QFont fontPetit;
    fontPetit.setPointSize(8);

    painter.setFont(fontPetit);

    QRect textRect(
        center.x() - 45,
        center.y() + 10,
        90,
        20
        );

    painter.drawText(
        textRect,
        Qt::AlignCenter,
        "techniciens"
        );


    // =========================
    // LEGENDE
    // =========================

    QFont legendFont;
    legendFont.setPointSize(9);

    painter.setFont(legendFont);

    // Actifs
    painter.setBrush(QColor("#18A66A"));
    painter.drawEllipse(235, 55, 9, 9);

    painter.setPen(QColor("#173F73"));

    painter.drawText(
        250,
        64,
        QString("Actifs  %1").arg(actifs)
        );


    // Inactifs
    painter.setBrush(QColor("#EF5350"));

    painter.drawEllipse(235, 90, 9, 9);

    painter.setPen(QColor("#173F73"));

    painter.drawText(
        250,
        99,
        QString("Inactifs  %1").arg(inactifs)
        );
}


// =====================================================
// CONSTRUCTEUR
// =====================================================

Technicienne::Technicienne(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    appliquerStyle();

    resize(1600, 900);

    setMinimumSize(1250, 750);

    setWindowTitle(
        "Smart Weather Management - Gestion des techniciens"
        );
}


// =====================================================
// DESTRUCTEUR
// =====================================================

Technicienne::~Technicienne()
{
}


// =====================================================
// SETUP PRINCIPAL
// =====================================================

void Technicienne::setupUI()
{
    QWidget *central = new QWidget(this);

    setCentralWidget(central);

    QHBoxLayout *mainLayout =
        new QHBoxLayout(central);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);


    // =========================
    // SIDEBAR
    // =========================

    setupSidebar();

    mainLayout->addWidget(sidebar);


    // =========================
    // CONTENU PRINCIPAL
    // =========================

    QWidget *content = new QWidget();

    QVBoxLayout *contentLayout =
        new QVBoxLayout(content);

    contentLayout->setContentsMargins(
        18, 0, 18, 8
        );

    contentLayout->setSpacing(8);


    // =========================
    // HEADER
    // =========================

    QWidget *header = new QWidget();

    header->setFixedHeight(70);

    QHBoxLayout *headerLayout =
        new QHBoxLayout(header);

    headerLayout->setContentsMargins(
        30, 0, 20, 0
        );


    QVBoxLayout *titleLayout =
        new QVBoxLayout();

    QLabel *title =
        new QLabel("Gestion des techniciens");

    title->setObjectName("pageTitle");

    QLabel *subtitle =
        new QLabel(
            "Gérez les techniciens et leurs informations"
            );

    subtitle->setObjectName("pageSubtitle");

    titleLayout->addWidget(title);
    titleLayout->addWidget(subtitle);

    headerLayout->addLayout(titleLayout);

    headerLayout->addStretch();


    QLabel *date =
        new QLabel("01/10/2026     15:22");

    QLabel *admin =
        new QLabel("●   Administrateur   ▼");

    date->setObjectName("headerInfo");
    admin->setObjectName("headerInfo");

    headerLayout->addWidget(date);
    headerLayout->addSpacing(20);
    headerLayout->addWidget(admin);


    contentLayout->addWidget(header);


    // =========================
    // INFORMATIONS + STATISTIQUES
    // =========================

    QHBoxLayout *topLayout =
        new QHBoxLayout();

    topLayout->setSpacing(10);


    // FORMULAIRE
    setupTechnicienForm();


    QFrame *formFrame = new QFrame();

    formFrame->setObjectName("card");

    formFrame->setMinimumHeight(300);
    formFrame->setMaximumHeight(315);

    QVBoxLayout *formLayout =
        new QVBoxLayout(formFrame);

    formLayout->setContentsMargins(
        15, 8, 15, 8
        );

    QLabel *formTitle =
        new QLabel("♟  Informations du technicien");

    formTitle->setObjectName("sectionTitle");

    formLayout->addWidget(formTitle);

    QGridLayout *grid =
        new QGridLayout();

    grid->setHorizontalSpacing(10);
    grid->setVerticalSpacing(7);


    // Ligne 1

    grid->addWidget(
        new QLabel("ID Technicien :"),
        0, 0
        );

    grid->addWidget(idTechnicien, 0, 1);

    grid->addWidget(
        new QLabel("Nom :"),
        0, 2
        );

    grid->addWidget(nom, 0, 3);


    // Ligne 2

    grid->addWidget(
        new QLabel("Prénom :"),
        1, 0
        );

    grid->addWidget(prenom, 1, 1);

    grid->addWidget(
        new QLabel("Email :"),
        1, 2
        );

    grid->addWidget(email, 1, 3);


    // Ligne 3

    grid->addWidget(
        new QLabel("Téléphone :"),
        2, 0
        );

    grid->addWidget(telephone, 2, 1);

    grid->addWidget(
        new QLabel("Formation :"),
        2, 2
        );

    grid->addWidget(formation, 2, 3);


    // Ligne 4

    grid->addWidget(
        new QLabel("Expérience :"),
        3, 0
        );

    grid->addWidget(experience, 3, 1);

    grid->addWidget(
        new QLabel("Spécialité :"),
        3, 2
        );

    grid->addWidget(specialite, 3, 3);


    // Ligne 5

    grid->addWidget(
        new QLabel("Statut :"),
        4, 0
        );

    grid->addWidget(statut, 4, 1);

    grid->addWidget(
        new QLabel("Date intégration :"),
        4, 2
        );

    grid->addWidget(dateIntegration, 4, 3);


    // Compétences

    grid->addWidget(
        new QLabel("Compétences :"),
        5, 0
        );

    grid->addWidget(
        competences,
        5, 1, 1, 3
        );


    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(3, 1);

    formLayout->addLayout(grid);


    // =========================
    // STATISTIQUES
    // =========================

    QFrame *statsFrame = new QFrame();

    statsFrame->setObjectName("card");

    statsFrame->setFixedWidth(330);
    statsFrame->setMinimumHeight(300);
    statsFrame->setMaximumHeight(315);

    QVBoxLayout *statsLayout =
        new QVBoxLayout(statsFrame);

    statsLayout->setContentsMargins(
        12, 8, 12, 5
        );

    QLabel *statsTitle =
        new QLabel("📊  Statistiques des techniciens");

    statsTitle->setObjectName("sectionTitle");

    statsLayout->addWidget(statsTitle);

    statisticsWidget =
        new StatisticsWidget();

    statsLayout->addWidget(
        statisticsWidget,
        1,
        Qt::AlignCenter
        );


    topLayout->addWidget(
        formFrame,
        1
        );

    topLayout->addWidget(
        statsFrame
        );


    contentLayout->addLayout(topLayout);


    // =========================
    // BOUTONS + RECHERCHE
    // =========================

    QFrame *toolsFrame = new QFrame();

    toolsFrame->setObjectName("card");

    toolsFrame->setFixedHeight(55);

    QHBoxLayout *toolsLayout =
        new QHBoxLayout(toolsFrame);

    toolsLayout->setContentsMargins(
        10, 7, 10, 7
        );

    btnAjouter =
        new QPushButton("＋  Ajouter");

    btnModifier =
        new QPushButton("✎  Modifier");

    btnSupprimer =
        new QPushButton("▣  Supprimer");

    btnVider =
        new QPushButton("↻  Vider");


    btnAjouter->setObjectName("btnAdd");
    btnModifier->setObjectName("btnModify");
    btnSupprimer->setObjectName("btnDelete");
    btnVider->setObjectName("btnClear");


    toolsLayout->addWidget(btnAjouter);
    toolsLayout->addWidget(btnModifier);
    toolsLayout->addWidget(btnSupprimer);
    toolsLayout->addWidget(btnVider);

    toolsLayout->addSpacing(12);


    QLabel *triLabel =
        new QLabel("↕  Trier par :");

    triCombo =
        new QComboBox();

    triCombo->addItems({
        "Nom",
        "Prénom",
        "Expérience",
        "Spécialité",
        "Statut"
    });

    triCombo->setFixedWidth(130);

    btnTrier =
        new QPushButton("Trier");

    btnTrier->setFixedWidth(70);


    toolsLayout->addWidget(triLabel);
    toolsLayout->addWidget(triCombo);
    toolsLayout->addWidget(btnTrier);


    recherche =
        new QLineEdit();

    recherche->setPlaceholderText(
        "Rechercher par ID, nom, spécialité..."
        );

    recherche->setFixedWidth(230);


    btnRechercher =
        new QPushButton("🔍");

    btnRechercher->setFixedWidth(50);


    btnImporterCV =
        new QPushButton("▣  Importer CV");

    btnImporterCV->setFixedWidth(130);


    toolsLayout->addWidget(recherche);
    toolsLayout->addWidget(btnRechercher);
    toolsLayout->addWidget(btnImporterCV);


    contentLayout->addWidget(toolsFrame);


    // =========================
    // TABLE
    // =========================

    setupTable();

    QFrame *tableFrame = new QFrame();

    tableFrame->setObjectName("card");

    tableFrame->setMinimumHeight(235);
    tableFrame->setMaximumHeight(250);

    QVBoxLayout *tableLayout =
        new QVBoxLayout(tableFrame);

    tableLayout->setContentsMargins(
        10, 6, 10, 5
        );

    QLabel *tableTitle =
        new QLabel("▦  Liste des techniciens");

    tableTitle->setObjectName("sectionTitle");

    tableLayout->addWidget(tableTitle);

    tableLayout->addWidget(table);


    contentLayout->addWidget(tableFrame);


    // =========================
    // METIERS
    // =========================

    setupMetiers();

    QFrame *metiersFrame =
        new QFrame();

    metiersFrame->setObjectName("card");

    metiersFrame->setFixedHeight(120);

    QVBoxLayout *metiersLayout =
        new QVBoxLayout(metiersFrame);

    metiersLayout->setContentsMargins(
        10, 6, 10, 8
        );


    QLabel *metiersTitle =
        new QLabel("⚙  Fonctions métiers");

    metiersTitle->setObjectName("sectionTitle");

    metiersLayout->addWidget(metiersTitle);


    QHBoxLayout *metiersButtons =
        new QHBoxLayout();

    metiersButtons->setSpacing(8);


    metiersButtons->addWidget(btnEntretien);
    metiersButtons->addWidget(btnQuestionnaire);
    metiersButtons->addWidget(btnCompetences);
    metiersButtons->addWidget(btnSpecialite);


    metiersLayout->addLayout(metiersButtons);

    contentLayout->addWidget(metiersFrame);


    mainLayout->addWidget(
        content,
        1
        );


    // =========================
    // SIGNALS
    // =========================

    connect(
        btnAjouter,
        &QPushButton::clicked,
        this,
        &Technicienne::ajouterTechnicien
        );

    connect(
        btnModifier,
        &QPushButton::clicked,
        this,
        &Technicienne::modifierTechnicien
        );

    connect(
        btnSupprimer,
        &QPushButton::clicked,
        this,
        &Technicienne::supprimerTechnicien
        );

    connect(
        btnVider,
        &QPushButton::clicked,
        this,
        &Technicienne::viderChamps
        );

    connect(
        btnRechercher,
        &QPushButton::clicked,
        this,
        &Technicienne::rechercherTechnicien
        );

    connect(
        btnTrier,
        &QPushButton::clicked,
        this,
        &Technicienne::trierTechnicien
        );

    connect(
        btnImporterCV,
        &QPushButton::clicked,
        this,
        &Technicienne::importerCV
        );

    connect(
        table,
        &QTableWidget::cellClicked,
        this,
        &Technicienne::selectionnerTechnicien
        );


    connect(
        btnEntretien,
        &QPushButton::clicked,
        this,
        &Technicienne::creerEntretien
        );

    connect(
        btnQuestionnaire,
        &QPushButton::clicked,
        this,
        &Technicienne::remplirQuestionnaire
        );

    connect(
        btnCompetences,
        &QPushButton::clicked,
        this,
        &Technicienne::evaluerCompetences
        );

    connect(
        btnSpecialite,
        &QPushButton::clicked,
        this,
        &Technicienne::determinerSpecialite
        );


    // =========================
    // DONNEES EXEMPLE
    // =========================

    QStringList ids = {
        "T001",
        "T002",
        "T003",
        "T004",
        "T005"
    };

    QStringList noms = {
        "Ben Ali",
        "Trabelsi",
        "Mansouri",
        "Saidi",
        "Zribi"
    };

    QStringList prenoms = {
        "Ahmed",
        "Sami",
        "Amel",
        "Yassine",
        "Walid"
    };

    QStringList emails = {
        "ahmed@gmail.com",
        "sami@gmail.com",
        "amel@gmail.com",
        "yassine@gmail.com",
        "walid@gmail.com"
    };

    QStringList phones = {
        "22111222",
        "22333444",
        "22888999",
        "22114567",
        "22557890"
    };

    QStringList formations = {
        "Informatique",
        "Informatique",
        "Météorologie",
        "Électronique",
        "Réseaux"
    };

    QStringList experiences = {
        "5",
        "3",
        "7",
        "4",
        "6"
    };

    QStringList specialites = {
        "Maintenance",
        "Informatique",
        "Météorologie",
        "Électronique",
        "Réseaux"
    };

    QStringList statuts = {
        "Actif",
        "Actif",
        "Actif",
        "Inactif",
        "Actif"
    };

    QStringList dates = {
        "15/09/2023",
        "10/01/2024",
        "22/03/2024",
        "12/06/2023",
        "30/11/2022"
    };


    for (int i = 0; i < ids.size(); i++)
    {
        int row = table->rowCount();

        table->insertRow(row);

        table->setItem(
            row, 0,
            new QTableWidgetItem(ids[i])
            );

        table->setItem(
            row, 1,
            new QTableWidgetItem(noms[i])
            );

        table->setItem(
            row, 2,
            new QTableWidgetItem(prenoms[i])
            );

        table->setItem(
            row, 3,
            new QTableWidgetItem(emails[i])
            );

        table->setItem(
            row, 4,
            new QTableWidgetItem(phones[i])
            );

        table->setItem(
            row, 5,
            new QTableWidgetItem(formations[i])
            );

        table->setItem(
            row, 6,
            new QTableWidgetItem(experiences[i])
            );

        table->setItem(
            row, 7,
            new QTableWidgetItem(specialites[i])
            );

        table->setItem(
            row, 8,
            new QTableWidgetItem(
                "Maintenance, analyse, diagnostic"
                )
            );

        table->setItem(
            row, 9,
            new QTableWidgetItem(statuts[i])
            );

        table->setItem(
            row, 10,
            new QTableWidgetItem(dates[i])
            );
    }


    updateStatistics();
}


// =====================================================
// SIDEBAR
// =====================================================

void Technicienne::setupSidebar()
{
    sidebar = new QWidget();

    sidebar->setObjectName("sidebar");

    sidebar->setFixedWidth(285);

    QVBoxLayout *layout =
        new QVBoxLayout(sidebar);

    layout->setContentsMargins(
        8, 20, 8, 15
        );

    layout->setSpacing(7);


    QLabel *logoIcon =
        new QLabel("☀");

    logoIcon->setAlignment(
        Qt::AlignCenter
        );

    logoIcon->setObjectName("logoIcon");

    layout->addWidget(logoIcon);


    QLabel *logoText =
        new QLabel(
            "SMART WEATHER\nMANAGEMENT"
            );

    logoText->setAlignment(
        Qt::AlignCenter
        );

    logoText->setObjectName("logoText");

    layout->addWidget(logoText);

    layout->addSpacing(20);


    btnAccueil =
        new QPushButton("⌂   Accueil");

    btnBureau =
        new QPushButton("▣   Gestion de bureau");

    btnEquipements =
        new QPushButton("⚙   Gestion des équipements");

    btnTechniciens =
        new QPushButton("♟   Gestion des techniciens");

    btnStations =
        new QPushButton("♨   Gestion des stations");

    btnUtilisateurs =
        new QPushButton("♟   Gestion des utilisateurs");


    QPushButton *buttons[] = {
        btnAccueil,
        btnBureau,
        btnEquipements,
        btnTechniciens,
        btnStations,
        btnUtilisateurs
    };


    for (QPushButton *button : buttons)
    {
        button->setObjectName("sideButton");

        button->setMinimumHeight(52);

        button->setCursor(
            Qt::PointingHandCursor
            );

        layout->addWidget(button);
    }


    btnTechniciens->setObjectName(
        "sideButtonSelected"
        );


    layout->addStretch();


    QLabel *bottom =
        new QLabel(
            "Données fiables\npour un avenir sûr"
            );

    bottom->setAlignment(
        Qt::AlignCenter
        );

    bottom->setObjectName(
        "sidebarBottom"
        );

    layout->addWidget(bottom);
}


// =====================================================
// FORMULAIRE
// =====================================================

void Technicienne::setupTechnicienForm()
{
    idTechnicien = new QLineEdit();
    nom = new QLineEdit();
    prenom = new QLineEdit();
    email = new QLineEdit();
    telephone = new QLineEdit();
    formation = new QLineEdit();

    experience = new QSpinBox();

    experience->setRange(0, 50);

    experience->setSuffix(" ans");

    specialite = new QLineEdit();

    competences = new QTextEdit();

    competences->setPlaceholderText(
        "Décrivez les compétences du technicien..."
        );

    competences->setMaximumHeight(50);


    statut = new QComboBox();

    statut->addItems({
        "Actif",
        "Inactif",
        "En formation"
    });


    dateIntegration = new QDateEdit();

    dateIntegration->setDate(
        QDate::currentDate()
        );

    dateIntegration->setCalendarPopup(true);


    QLineEdit *fields[] = {
        idTechnicien,
        nom,
        prenom,
        email,
        telephone,
        formation,
        specialite
    };


    for (QLineEdit *field : fields)
    {
        field->setMinimumHeight(34);
    }

    statut->setMinimumHeight(34);
    experience->setMinimumHeight(34);
    dateIntegration->setMinimumHeight(34);
}


// =====================================================
// STATISTIQUES
// =====================================================

void Technicienne::setupStatistics()
{
    // Les statistiques sont créées dans setupUI()
}


// =====================================================
// TABLE
// =====================================================

void Technicienne::setupTable()
{
    table = new QTableWidget();

    table->setColumnCount(11);

    table->setHorizontalHeaderLabels({
        "ID Technicien",
        "Nom",
        "Prénom",
        "Email",
        "Téléphone",
        "Formation",
        "Expérience",
        "Spécialité",
        "Compétences",
        "Statut",
        "Date intégration"
    });


    table->setSelectionBehavior(
        QAbstractItemView::SelectRows
        );

    table->setSelectionMode(
        QAbstractItemView::SingleSelection
        );

    table->setEditTriggers(
        QAbstractItemView::NoEditTriggers
        );

    table->setAlternatingRowColors(true);

    table->verticalHeader()->setVisible(false);

    table->horizontalHeader()->setStretchLastSection(true);

    table->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Interactive
        );

    table->setRowHeight(0, 30);


    // Largeurs

    table->setColumnWidth(0, 100);
    table->setColumnWidth(1, 105);
    table->setColumnWidth(2, 105);
    table->setColumnWidth(3, 190);
    table->setColumnWidth(4, 110);
    table->setColumnWidth(5, 130);
    table->setColumnWidth(6, 90);
    table->setColumnWidth(7, 130);
    table->setColumnWidth(8, 220);
    table->setColumnWidth(9, 90);
    table->setColumnWidth(10, 120);
}


// =====================================================
// METIERS
// =====================================================

void Technicienne::setupMetiers()
{
    btnEntretien =
        new QPushButton(
            "📅  Créer entretien"
            );

    btnQuestionnaire =
        new QPushButton(
            "▤  Remplir questionnaire"
            );

    btnCompetences =
        new QPushButton(
            "★  Évaluer compétences"
            );

    btnSpecialite =
        new QPushButton(
            "◎  Déterminer spécialité"
            );


    QPushButton *buttons[] = {
        btnEntretien,
        btnQuestionnaire,
        btnCompetences,
        btnSpecialite
    };


    for (QPushButton *button : buttons)
    {
        button->setMinimumHeight(55);
        button->setObjectName("metierButton");
        button->setCursor(
            Qt::PointingHandCursor
            );
    }
}


// =====================================================
// AJOUTER
// =====================================================

void Technicienne::ajouterTechnicien()
{
    if (idTechnicien->text().isEmpty() ||
        nom->text().isEmpty() ||
        prenom->text().isEmpty())
    {
        QMessageBox::warning(
            this,
            "Attention",
            "Veuillez remplir au minimum l'ID, le nom et le prénom."
            );

        return;
    }


    int row = table->rowCount();

    table->insertRow(row);


    table->setItem(
        row, 0,
        new QTableWidgetItem(
            idTechnicien->text()
            )
        );

    table->setItem(
        row, 1,
        new QTableWidgetItem(
            nom->text()
            )
        );

    table->setItem(
        row, 2,
        new QTableWidgetItem(
            prenom->text()
            )
        );

    table->setItem(
        row, 3,
        new QTableWidgetItem(
            email->text()
            )
        );

    table->setItem(
        row, 4,
        new QTableWidgetItem(
            telephone->text()
            )
        );

    table->setItem(
        row, 5,
        new QTableWidgetItem(
            formation->text()
            )
        );

    table->setItem(
        row, 6,
        new QTableWidgetItem(
            QString::number(
                experience->value()
                )
            )
        );

    table->setItem(
        row, 7,
        new QTableWidgetItem(
            specialite->text()
            )
        );

    table->setItem(
        row, 8,
        new QTableWidgetItem(
            competences->toPlainText()
            )
        );

    table->setItem(
        row, 9,
        new QTableWidgetItem(
            statut->currentText()
            )
        );

    table->setItem(
        row, 10,
        new QTableWidgetItem(
            dateIntegration->date().toString(
                "dd/MM/yyyy"
                )
            )
        );


    updateStatistics();

    QMessageBox::information(
        this,
        "Ajout",
        "Technicien ajouté avec succès."
        );

    viderChamps();
}


// =====================================================
// MODIFIER
// =====================================================

void Technicienne::modifierTechnicien()
{
    int row =
        table->currentRow();

    if (row < 0)
    {
        QMessageBox::warning(
            this,
            "Attention",
            "Sélectionnez un technicien dans la liste."
            );

        return;
    }


    table->item(row, 0)->setText(
        idTechnicien->text()
        );

    table->item(row, 1)->setText(
        nom->text()
        );

    table->item(row, 2)->setText(
        prenom->text()
        );

    table->item(row, 3)->setText(
        email->text()
        );

    table->item(row, 4)->setText(
        telephone->text()
        );

    table->item(row, 5)->setText(
        formation->text()
        );

    table->item(row, 6)->setText(
        QString::number(
            experience->value()
            )
        );

    table->item(row, 7)->setText(
        specialite->text()
        );

    table->item(row, 8)->setText(
        competences->toPlainText()
        );

    table->item(row, 9)->setText(
        statut->currentText()
        );

    table->item(row, 10)->setText(
        dateIntegration->date().toString(
            "dd/MM/yyyy"
            )
        );


    updateStatistics();

    QMessageBox::information(
        this,
        "Modification",
        "Technicien modifié avec succès."
        );
}


// =====================================================
// SUPPRIMER
// =====================================================

void Technicienne::supprimerTechnicien()
{
    int row =
        table->currentRow();

    if (row < 0)
    {
        QMessageBox::warning(
            this,
            "Attention",
            "Sélectionnez un technicien."
            );

        return;
    }


    QMessageBox::StandardButton answer =
        QMessageBox::question(
            this,
            "Confirmation",
            "Voulez-vous supprimer ce technicien ?"
            );


    if (answer ==
        QMessageBox::Yes)
    {
        table->removeRow(row);

        updateStatistics();

        viderChamps();
    }
}


// =====================================================
// VIDER
// =====================================================

void Technicienne::viderChamps()
{
    idTechnicien->clear();
    nom->clear();
    prenom->clear();
    email->clear();
    telephone->clear();
    formation->clear();
    specialite->clear();

    experience->setValue(0);

    competences->clear();

    statut->setCurrentIndex(0);

    dateIntegration->setDate(
        QDate::currentDate()
        );

    table->clearSelection();
}


// =====================================================
// RECHERCHER
// =====================================================

void Technicienne::rechercherTechnicien()
{
    QString text =
        recherche->text()
            .trimmed()
            .toLower();


    for (int row = 0;
         row < table->rowCount();
         row++)
    {
        bool found = false;


        for (int column = 0;
             column < table->columnCount();
             column++)
        {
            QTableWidgetItem *item =
                table->item(row, column);

            if (item &&
                item->text()
                    .toLower()
                    .contains(text))
            {
                found = true;
                break;
            }
        }


        table->setRowHidden(
            row,
            !found
            );
    }
}


// =====================================================
// TRIER
// =====================================================

void Technicienne::trierTechnicien()
{
    int column = 1;


    QString value =
        triCombo->currentText();


    if (value == "Nom")
        column = 1;

    else if (value == "Prénom")
        column = 2;

    else if (value == "Expérience")
        column = 6;

    else if (value == "Spécialité")
        column = 7;

    else if (value == "Statut")
        column = 9;


    table->sortItems(
        column,
        Qt::AscendingOrder
        );
}


// =====================================================
// IMPORTER CV
// =====================================================

void Technicienne::importerCV()
{
    QString fileName =
        QFileDialog::getOpenFileName(
            this,
            "Importer un CV",
            "",
            "Documents (*.pdf *.doc *.docx);;Tous les fichiers (*)"
            );


    if (fileName.isEmpty())
        return;


    QMessageBox::information(
        this,
        "Importation du CV",
        "Le CV a été sélectionné avec succès.\n\n"
        "Fichier :\n" + fileName
        );
}


// =====================================================
// SELECTION TABLE
// =====================================================

void Technicienne::selectionnerTechnicien(
    int row,
    int column
    )
{
    Q_UNUSED(column);


    if (row < 0)
        return;


    idTechnicien->setText(
        table->item(row, 0)->text()
        );

    nom->setText(
        table->item(row, 1)->text()
        );

    prenom->setText(
        table->item(row, 2)->text()
        );

    email->setText(
        table->item(row, 3)->text()
        );

    telephone->setText(
        table->item(row, 4)->text()
        );

    formation->setText(
        table->item(row, 5)->text()
        );

    experience->setValue(
        table->item(row, 6)->text().toInt()
        );

    specialite->setText(
        table->item(row, 7)->text()
        );

    competences->setPlainText(
        table->item(row, 8)->text()
        );


    int statusIndex =
        statut->findText(
            table->item(row, 9)->text()
            );

    if (statusIndex >= 0)
        statut->setCurrentIndex(
            statusIndex
            );


    dateIntegration->setDate(
        QDate::fromString(
            table->item(row, 10)->text(),
            "dd/MM/yyyy"
            )
        );
}


// =====================================================
// CREER ENTRETIEN
// =====================================================

void Technicienne::creerEntretien()
{
    if (table->currentRow() < 0)
    {
        QMessageBox::information(
            this,
            "Entretien",
            "Sélectionnez d'abord un technicien."
            );

        return;
    }


    QMessageBox::information(
        this,
        "Créer entretien",
        "Entretien créé pour :\n" +
            nom->text() + " " +
            prenom->text()
        );
}


// =====================================================
// QUESTIONNAIRE
// =====================================================

void Technicienne::remplirQuestionnaire()
{
    if (table->currentRow() < 0)
    {
        QMessageBox::information(
            this,
            "Questionnaire",
            "Sélectionnez d'abord un technicien."
            );

        return;
    }


    bool ok;

    QString answer =
        QInputDialog::getText(
            this,
            "Questionnaire",
            "Compétence principale du technicien :",
            QLineEdit::Normal,
            "",
            &ok
            );


    if (ok && !answer.isEmpty())
    {
        competences->setPlainText(
            answer
            );
    }
}


// =====================================================
// EVALUER COMPETENCES
// =====================================================

void Technicienne::evaluerCompetences()
{
    if (table->currentRow() < 0)
    {
        QMessageBox::information(
            this,
            "Évaluation",
            "Sélectionnez d'abord un technicien."
            );

        return;
    }


    bool ok;

    int note =
        QInputDialog::getInt(
            this,
            "Évaluer compétences",
            "Note sur 100 :",
            80,
            0,
            100,
            1,
            &ok
            );


    if (ok)
    {
        QMessageBox::information(
            this,
            "Évaluation",
            QString(
                "Évaluation enregistrée : %1/100"
                ).arg(note)
            );
    }
}


// =====================================================
// DETERMINER SPECIALITE
// =====================================================

void Technicienne::determinerSpecialite()
{
    if (table->currentRow() < 0)
    {
        QMessageBox::information(
            this,
            "Spécialité",
            "Sélectionnez d'abord un technicien."
            );

        return;
    }


    QString result;


    QString text =
        competences->toPlainText()
            .toLower();


    if (text.contains("réseau"))
        result = "Réseaux";

    else if (text.contains("maintenance"))
        result = "Maintenance";

    else if (text.contains("météo"))
        result = "Météorologie";

    else if (text.contains("électronique"))
        result = "Électronique";

    else
        result = "Informatique";


    specialite->setText(
        result
        );


    QMessageBox::information(
        this,
        "Spécialité déterminée",
        "Spécialité proposée : " +
            result
        );
}


// =====================================================
// STATISTIQUES
// =====================================================

void Technicienne::updateStatistics()
{
    int actifsCount = 0;
    int inactifsCount = 0;


    for (int row = 0;
         row < table->rowCount();
         row++)
    {
        QTableWidgetItem *item =
            table->item(row, 9);


        if (!item)
            continue;


        if (item->text() == "Actif")
            actifsCount++;

        else if (item->text() == "Inactif")
            inactifsCount++;
    }


    statisticsWidget->setStatistics(
        actifsCount,
        inactifsCount
        );
}


// =====================================================
// STYLE GRAPHIQUE
// =====================================================

void Technicienne::appliquerStyle()
{
    setStyleSheet(R"(

        * {
            font-family: "Segoe UI";
            font-size: 13px;
        }


        QMainWindow {
            background: #F4F7FB;
        }


        /* =========================
           SIDEBAR
           ========================= */

        #sidebar {
            background: #17467D;
        }


        #logoIcon {
            color: #FFD21F;
            font-size: 40px;
            font-weight: bold;
        }


        #logoText {
            color: white;
            font-size: 17px;
            font-weight: bold;
        }


        #sideButton,
        #sideButtonSelected {
            border: none;
            border-radius: 7px;
            text-align: left;
            padding-left: 18px;
            color: white;
            font-size: 14px;
            background: transparent;
        }


        #sideButton:hover {
            background: #2468B4;
        }


        #sideButtonSelected {
            background: #287ED1;
            font-weight: bold;
        }


        #sidebarBottom {
            color: white;
            font-size: 12px;
        }


        /* =========================
           HEADER
           ========================= */

        #pageTitle {
            color: #FFFFFF;
            background: transparent;
            font-size: 26px;
            font-weight: bold;
            padding: 0px;
        }


        #pageSubtitle {
            color: #E5F0FF;
            background: transparent;
            font-size: 13px;
        }


        #headerInfo {
            color: white;
            background: transparent;
            font-size: 13px;
        }


        /* =========================
           CARDS
           ========================= */

        #card {
            background: white;
            border: 1px solid #D5E1EF;
            border-radius: 8px;
        }


        #sectionTitle {
            color: #123E70;
            font-size: 15px;
            font-weight: bold;
            padding: 2px;
        }


        QLabel {
            color: #173F73;
        }


        /* =========================
           INPUTS
           ========================= */

        QLineEdit,
        QComboBox,
        QSpinBox,
        QDateEdit,
        QTextEdit {
            background: white;
            border: 1px solid #BFD3E8;
            border-radius: 5px;
            padding: 6px;
            color: #173F73;
        }


        QLineEdit:focus,
        QComboBox:focus,
        QSpinBox:focus,
        QDateEdit:focus,
        QTextEdit:focus {
            border: 2px solid #287ED1;
        }


        /* =========================
           BOUTONS
           ========================= */

        QPushButton {
            border-radius: 5px;
            padding: 8px 15px;
            border: 1px solid #C5D6E8;
            background: white;
            color: #173F73;
        }


        QPushButton:hover {
            background: #EEF6FF;
        }


        #btnAdd {
            background: #287ED1;
            color: white;
            border: none;
            font-weight: bold;
        }


        #btnAdd:hover {
            background: #1768B7;
        }


        #btnModify {
            background: white;
            color: #173F73;
        }


        #btnDelete {
            background: white;
            color: #D83434;
        }


        #btnClear {
            background: white;
            color: #173F73;
        }


        #metierButton {
            background: white;
            border: 1px solid #BFD3E8;
            color: #173F73;
            font-weight: bold;
            text-align: left;
            padding-left: 15px;
        }


        #metierButton:hover {
            background: #EEF6FF;
            border: 1px solid #287ED1;
        }


        /* =========================
           TABLE
           ========================= */

        QTableWidget {
            background: white;
            alternate-background-color: #F7FAFD;
            border: 1px solid #D6E2EF;
            gridline-color: #DDE7F0;
            color: #173F73;
            selection-background-color: #DCEEFF;
            selection-color: #123E70;
        }


        QHeaderView::section {
            background: #EDF3FA;
            color: #173F73;
            font-weight: bold;
            border: none;
            border-right: 1px solid #D5E1EF;
            padding: 7px;
        }


        QTableWidget::item {
            padding: 5px;
        }


        /* =========================
           COMBO
           ========================= */

        QComboBox QAbstractItemView {
            background: white;
            color: #173F73;
            selection-background-color: #287ED1;
            selection-color: white;
        }

    )");


    // =========================
    // HEADER BLEU
    // =========================

    QWidget *central =
        centralWidget();

    if (central)
    {
        // On applique le bleu au header
        QList<QWidget*> children =
            central->findChildren<QWidget*>();

        for (QWidget *w : children)
        {
            if (w->height() == 70 &&
                w->objectName().isEmpty())
            {
                w->setStyleSheet(
                    "background:#287ED1;"
                    );

                break;
            }
        }
    }
}
