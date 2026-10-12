#include "technicienne.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QFontMetrics>
#include <QColor>

#include <QMessageBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QTableWidgetItem>

#include <QPrinter>
#include <QTextDocument>
#include <QRegularExpression>

#include <QDate>
#include <QTime>
#include <QTimer>
#include <QVector>
#include <algorithm>


// Indices des colonnes du tableau
namespace
{
const int COL_ID          = 0;
const int COL_NOM         = 1;
const int COL_PRENOM      = 2;
const int COL_EMAIL       = 3;
const int COL_TELEPHONE   = 4;
const int COL_FORMATION   = 5;
const int COL_EXPERIENCE  = 6;
const int COL_SPECIALITE  = 7;
const int COL_COMPETENCES = 8;
const int COL_STATUT      = 9;
const int COL_DATE        = 10;
const int NB_COLONNES     = 11;

QString texteCellule(const QTableWidget *table, int row, int col)
{
    const QTableWidgetItem *it = table->item(row, col);
    return it ? it->text() : QString();
}
}


// ============================================================
// TECH STATISTICS WIDGET
// ============================================================

TechStatisticsWidget::TechStatisticsWidget(QWidget *parent)
    : QWidget(parent),
    m_actifs(0),
    m_inactifs(0)
{
    setMinimumHeight(170);
}


void TechStatisticsWidget::setStatistics(int actifs, int inactifs)
{
    m_actifs = actifs;
    m_inactifs = inactifs;

    update();
}


void TechStatisticsWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const int total = m_actifs + m_inactifs;

    const QFont legendFont("Segoe UI", 9);
    const QFontMetrics fm(legendFont);

    struct Item
    {
        QColor color;
        QString text;
    };

    QVector<Item> items;

    if (total == 0)
    {
        Item it;
        it.color = QColor("#DCE8F5");
        it.text = "Aucun technicien";
        items.append(it);
    }
    else
    {
        Item a;
        a.color = QColor("#16A765");
        a.text = QString("Actifs : %1 (%2%)")
                     .arg(m_actifs)
                     .arg(static_cast<double>(m_actifs) / total * 100.0, 0, 'f', 1);
        items.append(a);

        Item b;
        b.color = QColor("#EF4444");
        b.text = QString("Inactifs : %1 (%2%)")
                     .arg(m_inactifs)
                     .arg(static_cast<double>(m_inactifs) / total * 100.0, 0, 'f', 1);
        items.append(b);
    }

    int textWidth = 0;
    for (int i = 0; i < items.size(); ++i)
        textWidth = std::max(textWidth, fm.horizontalAdvance(items[i].text));

    const int dot = 10;
    const int dotGap = 8;
    const int gap = 24;
    const int legendWidth = dot + dotGap + textWidth;

    int diameter = std::min(height() - 10, 150);
    diameter = std::min(diameter, width() - legendWidth - gap - 10);
    diameter = std::max(diameter, 60);

    const int blockWidth = diameter + gap + legendWidth;
    const double x0 = (width() - blockWidth) / 2.0;
    const double y0 = (height() - diameter) / 2.0;

    const QRectF pieRect(x0, y0, diameter, diameter);
    const double holeSize = diameter * 0.62;
    const QRectF holeRect(pieRect.center().x() - holeSize / 2.0,
                          pieRect.center().y() - holeSize / 2.0,
                          holeSize,
                          holeSize);

    if (total == 0)
    {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(QColor("#DCE8F5"), 12));
        painter.drawEllipse(pieRect.adjusted(6, 6, -6, -6));
    }
    else
    {
        const int startAngle = 90 * 16;
        const int actifAngle =
            qRound(static_cast<double>(m_actifs) / total * 360 * 16);
        const int inactifAngle = 360 * 16 - actifAngle;

        painter.setPen(Qt::NoPen);

        painter.setBrush(QColor("#16A765"));
        painter.drawPie(pieRect, startAngle, -actifAngle);

        painter.setBrush(QColor("#EF4444"));
        painter.drawPie(pieRect, startAngle - actifAngle, -inactifAngle);

        painter.setBrush(Qt::white);
        painter.drawEllipse(holeRect);
    }

    // Texte au centre
    painter.setPen(QColor("#173B6C"));
    painter.setFont(QFont("Segoe UI", 22, QFont::Bold));
    painter.drawText(
        QRectF(holeRect.left(), holeRect.center().y() - 28, holeRect.width(), 32),
        Qt::AlignCenter,
        QString::number(total));

    painter.setFont(QFont("Segoe UI", 9));
    painter.drawText(
        QRectF(holeRect.left(), holeRect.center().y() + 4, holeRect.width(), 20),
        Qt::AlignCenter,
        "techniciens");

    // Légende
    painter.setFont(legendFont);

    const int rowHeight = 28;
    const double legendX = x0 + diameter + gap;
    double rowY = height() / 2.0 - (items.size() * rowHeight) / 2.0;

    for (int i = 0; i < items.size(); ++i)
    {
        const double cy = rowY + rowHeight / 2.0;

        painter.setPen(Qt::NoPen);
        painter.setBrush(items[i].color);
        painter.drawEllipse(QRectF(legendX, cy - dot / 2.0, dot, dot));

        painter.setPen(QColor("#234A78"));
        painter.drawText(
            QRectF(legendX + dot + dotGap, rowY, textWidth + 4, rowHeight),
            Qt::AlignVCenter | Qt::AlignLeft,
            items[i].text);

        rowY += rowHeight;
    }
}


// ============================================================
// FENETRE TECHNICIENNE
// ============================================================

Technicienne::Technicienne(QWidget *parent)
    : QMainWindow(parent),
    sidebar(nullptr),
    btnAccueil(nullptr),
    btnBureau(nullptr),
    btnEquipements(nullptr),
    btnTechniciens(nullptr),
    btnStations(nullptr),
    btnUtilisateurs(nullptr),
    idTechnicien(nullptr),
    nom(nullptr),
    prenom(nullptr),
    email(nullptr),
    telephone(nullptr),
    formation(nullptr),
    experience(nullptr),
    specialite(nullptr),
    competences(nullptr),
    statut(nullptr),
    dateIntegration(nullptr),
    btnAjouter(nullptr),
    btnModifier(nullptr),
    btnSupprimer(nullptr),
    btnVider(nullptr),
    recherche(nullptr),
    triCombo(nullptr),
    btnTrier(nullptr),
    btnRechercher(nullptr),
    btnExporterPDF(nullptr),
    table(nullptr),
    statisticsWidget(nullptr),
    lblTotal(nullptr),
    lblActifs(nullptr),
    lblInactifs(nullptr)
{
    setWindowTitle("SmartWeather - Gestion des techniciens");

    resize(1450, 850);
    setMinimumSize(1200, 700);

    setupUI();
    appliquerStyle();

    chargerDonneesDemo();
    updateStatistics();
}


Technicienne::~Technicienne()
{
}


// ============================================================
// SETUP UI
// ============================================================

void Technicienne::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Barre latérale
    QWidget *side = setupSidebar();
    side->setFixedWidth(235);
    mainLayout->addWidget(side);

    // Partie droite
    QWidget *content = new QWidget;
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(20, 0, 20, 15);
    contentLayout->setSpacing(12);

    contentLayout->addWidget(setupHeader());
    contentLayout->addWidget(setupTechnicienForm());
    contentLayout->addWidget(setupActionBar());

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->setSpacing(12);

    bottomLayout->addWidget(setupTable(), 7);
    bottomLayout->addWidget(setupStatistics(), 3);

    contentLayout->addLayout(bottomLayout, 1);

    mainLayout->addWidget(content, 1);
}


// ============================================================
// SIDEBAR
// ============================================================

QWidget *Technicienne::setupSidebar()
{
    sidebar = new QWidget;
    sidebar->setObjectName("sidebar");

    QVBoxLayout *layout = new QVBoxLayout(sidebar);
    layout->setContentsMargins(10, 25, 10, 20);
    layout->setSpacing(8);

    QLabel *logo = new QLabel(QString::fromUtf8("☀"));
    logo->setAlignment(Qt::AlignCenter);
    logo->setStyleSheet("font-size: 48px; color: #FFD21F;");
    layout->addWidget(logo);

    QLabel *title = new QLabel("SMART WEATHER\nMANAGEMENT");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "color: white;"
        "font-size: 18px;"
        "font-weight: bold;"
        "padding-bottom: 30px;");
    layout->addWidget(title);

    btnAccueil = new QPushButton(QString::fromUtf8("⌂   Accueil"));
    btnAccueil->setObjectName("sideButton");
    layout->addWidget(btnAccueil);

    QPushButton *btnBureaux = new QPushButton(QString::fromUtf8("🏢   Gestion des bureaux"));
    btnBureaux->setObjectName("sideButton");
    layout->addWidget(btnBureaux);

    btnEquipements = new QPushButton(QString::fromUtf8("⚙   Gestion des équipements"));
    btnEquipements->setObjectName("sideButton");
    layout->addWidget(btnEquipements);

    btnTechniciens = new QPushButton(QString::fromUtf8("♟   Gestion des techniciens"));
    btnTechniciens->setObjectName("sideButtonActive");
    layout->addWidget(btnTechniciens);

    btnStations = new QPushButton(QString::fromUtf8("☁   Gestion des stations"));
    btnStations->setObjectName("sideButton");
    layout->addWidget(btnStations);

    btnUtilisateurs = new QPushButton(QString::fromUtf8("♟   Gestion des utilisateurs"));
    btnUtilisateurs->setObjectName("sideButton");
    layout->addWidget(btnUtilisateurs);

    layout->addStretch();

    btnBureau = new QPushButton(QString::fromUtf8("⚙   Paramètres"));
    btnBureau->setObjectName("sideButton");
    layout->addWidget(btnBureau);

    // Navigation : clic sur "Gestion des stations" -> interface des stations
    connect(btnStations, &QPushButton::clicked,
            this, &Technicienne::stationsRequested);

    return sidebar;
}


// ============================================================
// HEADER
// ============================================================

QWidget *Technicienne::setupHeader()
{
    QWidget *header = new QWidget;
    header->setObjectName("header");
    header->setFixedHeight(90);

    QHBoxLayout *layout = new QHBoxLayout(header);
    layout->setContentsMargins(25, 8, 25, 8);

    QVBoxLayout *titles = new QVBoxLayout;

    QLabel *title = new QLabel("Gestion des techniciens");
    title->setObjectName("headerTitle");

    QLabel *subtitle = new QLabel(
        "Gérez les techniciens et leurs informations");
    subtitle->setObjectName("headerSubtitle");

    titles->addWidget(title);
    titles->addWidget(subtitle);

    layout->addLayout(titles);
    layout->addStretch();

    QLabel *date = new QLabel(QDate::currentDate().toString("dd/MM/yyyy"));
    date->setStyleSheet("color: white; font-weight: bold; font-size: 14px;");

    QLabel *separator = new QLabel(QString::fromUtf8("  •  "));
    separator->setStyleSheet("color: white;");

    QLabel *admin = new QLabel(QString::fromUtf8("Administrateur ▼"));
    admin->setStyleSheet("color: white; font-weight: bold;");

    layout->addWidget(date);

    QLabel *clock = new QLabel(QTime::currentTime().toString("HH:mm"));
    clock->setStyleSheet(
        "color: white; font-weight: bold; font-size: 14px; padding-left: 14px;");

    QTimer *clockTimer = new QTimer(clock);
    connect(clockTimer, &QTimer::timeout, clock,
            [clock]() { clock->setText(QTime::currentTime().toString("HH:mm")); });
    clockTimer->start(1000);

    layout->addWidget(clock);
    layout->addWidget(separator);
    layout->addWidget(admin);

    return header;
}


// ============================================================
// FORMULAIRE TECHNICIEN
// ============================================================

QGroupBox *Technicienne::setupTechnicienForm()
{
    QGroupBox *box = new QGroupBox(QString::fromUtf8("♟   Informations du technicien"));

    QGridLayout *layout = new QGridLayout(box);
    layout->setContentsMargins(20, 25, 20, 18);
    layout->setHorizontalSpacing(12);
    layout->setVerticalSpacing(10);

    idTechnicien = new QLineEdit;
    idTechnicien->setPlaceholderText("Ex : 1");

    nom = new QLineEdit;
    nom->setPlaceholderText("Nom");

    prenom = new QLineEdit;
    prenom->setPlaceholderText("Prénom");

    email = new QLineEdit;
    email->setPlaceholderText("exemple@mail.com");

    telephone = new QLineEdit;
    telephone->setPlaceholderText("Ex : 20123456");

    formation = new QLineEdit;
    formation->setPlaceholderText("Formation");

    experience = new QSpinBox;
    experience->setRange(0, 60);
    experience->setSuffix(" an(s)");

    specialite = new QLineEdit;
    specialite->setPlaceholderText("Spécialité");

    statut = new QComboBox;
    statut->addItem("Actif");
    statut->addItem("Inactif");

    dateIntegration = new QDateEdit(QDate::currentDate());
    dateIntegration->setCalendarPopup(true);
    dateIntegration->setDisplayFormat("dd/MM/yyyy");

    competences = new QTextEdit;
    competences->setPlaceholderText("Compétences (séparées par des virgules)");
    competences->setFixedHeight(64);
    competences->setAcceptRichText(false);

    // Ligne 0
    layout->addWidget(new QLabel("ID :"), 0, 0);
    layout->addWidget(idTechnicien, 0, 1);
    layout->addWidget(new QLabel("Nom :"), 0, 2);
    layout->addWidget(nom, 0, 3);
    layout->addWidget(new QLabel("Prénom :"), 0, 4);
    layout->addWidget(prenom, 0, 5);

    // Ligne 1
    layout->addWidget(new QLabel("Email :"), 1, 0);
    layout->addWidget(email, 1, 1);
    layout->addWidget(new QLabel("Téléphone :"), 1, 2);
    layout->addWidget(telephone, 1, 3);
    layout->addWidget(new QLabel("Formation :"), 1, 4);
    layout->addWidget(formation, 1, 5);

    // Ligne 2
    layout->addWidget(new QLabel("Expérience :"), 2, 0);
    layout->addWidget(experience, 2, 1);
    layout->addWidget(new QLabel("Spécialité :"), 2, 2);
    layout->addWidget(specialite, 2, 3);
    layout->addWidget(new QLabel("Statut :"), 2, 4);
    layout->addWidget(statut, 2, 5);

    // Ligne 3
    layout->addWidget(new QLabel("Date d'intégration :"), 3, 0);
    layout->addWidget(dateIntegration, 3, 1);
    layout->addWidget(new QLabel("Compétences :"), 3, 2);
    layout->addWidget(competences, 3, 3, 1, 3);

    layout->setColumnStretch(1, 1);
    layout->setColumnStretch(3, 1);
    layout->setColumnStretch(5, 1);

    return box;
}


// ============================================================
// BARRE D'ACTIONS (CRUD + recherche / tri / export)
// ============================================================

QWidget *Technicienne::setupActionBar()
{
    QGroupBox *box = new QGroupBox(QString::fromUtf8("⚡   Actions"));

    QHBoxLayout *layout = new QHBoxLayout(box);
    layout->setContentsMargins(20, 22, 20, 14);
    layout->setSpacing(10);

    btnAjouter = new QPushButton("+  Ajouter");
    btnAjouter->setObjectName("primaryButton");

    btnModifier = new QPushButton("Modifier");

    btnSupprimer = new QPushButton("Supprimer");
    btnSupprimer->setObjectName("deleteButton");

    btnVider = new QPushButton("Vider");

    recherche = new QLineEdit;
    recherche->setPlaceholderText("Rechercher un technicien...");
    recherche->setMinimumWidth(220);

    btnRechercher = new QPushButton("Rechercher");

    triCombo = new QComboBox;
    triCombo->addItem("ID");
    triCombo->addItem("Nom");
    triCombo->addItem("Prénom");
    triCombo->addItem("Expérience");
    triCombo->addItem("Statut");
    triCombo->addItem("Date d'intégration");

    btnTrier = new QPushButton("Trier");

    btnExporterPDF = new QPushButton("Exporter PDF");

    layout->addWidget(btnAjouter);
    layout->addWidget(btnModifier);
    layout->addWidget(btnSupprimer);
    layout->addWidget(btnVider);
    layout->addStretch();
    layout->addWidget(recherche);
    layout->addWidget(btnRechercher);
    layout->addWidget(triCombo);
    layout->addWidget(btnTrier);
    layout->addWidget(btnExporterPDF);

    connect(btnAjouter, &QPushButton::clicked,
            this, [this]() { ajouterTechnicien(); });
    connect(btnModifier, &QPushButton::clicked,
            this, [this]() { modifierTechnicien(); });
    connect(btnSupprimer, &QPushButton::clicked,
            this, [this]() { supprimerTechnicien(); });
    connect(btnVider, &QPushButton::clicked,
            this, [this]() { viderChamps(); });
    connect(btnRechercher, &QPushButton::clicked,
            this, [this]() { rechercherTechnicien(); });
    connect(recherche, &QLineEdit::returnPressed,
            this, [this]() { rechercherTechnicien(); });
    connect(btnTrier, &QPushButton::clicked,
            this, [this]() { trierTechnicien(); });
    connect(btnExporterPDF, &QPushButton::clicked,
            this, [this]() { exporterPDF(); });

    return box;
}


// ============================================================
// TABLE
// ============================================================

QGroupBox *Technicienne::setupTable()
{
    QGroupBox *box = new QGroupBox(QString::fromUtf8("▤   Liste des techniciens"));

    QVBoxLayout *layout = new QVBoxLayout(box);
    layout->setContentsMargins(15, 22, 15, 12);

    QLabel *instruction = new QLabel(
        "Sélectionnez une ligne pour modifier les informations");
    instruction->setAlignment(Qt::AlignRight);
    instruction->setStyleSheet("color: #7B8CA5; font-size: 11px;");
    layout->addWidget(instruction);

    table = new QTableWidget;
    table->setColumnCount(NB_COLONNES);
    table->setHorizontalHeaderLabels(
        QStringList()
        << "ID" << "Nom" << "Prénom" << "Email" << "Téléphone"
        << "Formation" << "Expérience" << "Spécialité"
        << "Compétences" << "Statut" << "Date d'intégration");

    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    table->setColumnWidth(COL_ID, 50);
    table->setColumnWidth(COL_NOM, 100);
    table->setColumnWidth(COL_PRENOM, 100);
    table->setColumnWidth(COL_EMAIL, 170);
    table->setColumnWidth(COL_TELEPHONE, 95);
    table->setColumnWidth(COL_FORMATION, 120);
    table->setColumnWidth(COL_EXPERIENCE, 90);
    table->setColumnWidth(COL_SPECIALITE, 110);
    table->setColumnWidth(COL_COMPETENCES, 150);
    table->setColumnWidth(COL_STATUT, 75);

    layout->addWidget(table);

    connect(table, &QTableWidget::cellClicked,
            this, &Technicienne::selectionnerTechnicien);

    return box;
}


// ============================================================
// STATISTIQUES
// ============================================================

QGroupBox *Technicienne::setupStatistics()
{
    QGroupBox *box = new QGroupBox(QString::fromUtf8("◔   Statistiques"));

    QVBoxLayout *layout = new QVBoxLayout(box);
    layout->setContentsMargins(15, 25, 15, 15);
    layout->setSpacing(10);

    statisticsWidget = new TechStatisticsWidget;
    layout->addWidget(statisticsWidget, 1);

    lblTotal = new QLabel("Total : 0");
    lblActifs = new QLabel("Actifs : 0");
    lblInactifs = new QLabel("Inactifs : 0");

    lblTotal->setStyleSheet("font-weight: bold; color: #173B6C;");
    lblActifs->setStyleSheet("font-weight: bold; color: #16A765;");
    lblInactifs->setStyleSheet("font-weight: bold; color: #EF4444;");

    layout->addWidget(lblTotal);
    layout->addWidget(lblActifs);
    layout->addWidget(lblInactifs);
    layout->addStretch();

    return box;
}


// ============================================================
// STYLE
// ============================================================

void Technicienne::appliquerStyle()
{
    const QString style = R"(
        QMainWindow {
            background: #F4F7FB;
        }

        QWidget {
            font-family: 'Segoe UI', Arial;
            font-size: 13px;
            color: #173B6C;
        }

        #sidebar {
            background: #174481;
        }

        #sideButton,
        #sideButtonActive {
            border: none;
            border-radius: 7px;
            padding: 14px 12px;
            text-align: left;
            font-size: 15px;
        }

        #sideButton {
            background: transparent;
            color: white;
        }

        #sideButton:hover {
            background: #24589A;
        }

        #sideButtonActive {
            background: #2E7BD6;
            color: white;
            font-weight: bold;
        }

        #header {
            background: #287ACB;
            border-bottom-left-radius: 2px;
            border-bottom-right-radius: 2px;
        }

        #headerTitle {
            color: white;
            font-size: 30px;
            font-weight: bold;
        }

        #headerSubtitle {
            color: #E5F1FF;
            font-size: 12px;
        }

        QGroupBox {
            background: white;
            border: 1px solid #D5E1EF;
            border-radius: 9px;
            margin-top: 5px;
            font-weight: bold;
            font-size: 16px;
            color: #173B6C;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 7px;
            background: white;
        }

        QLineEdit,
        QSpinBox,
        QDateEdit,
        QComboBox,
        QTextEdit {
            background: white;
            border: 1px solid #C9D8E9;
            border-radius: 6px;
            padding: 6px;
            min-height: 18px;
        }

        QLineEdit:focus,
        QSpinBox:focus,
        QDateEdit:focus,
        QComboBox:focus,
        QTextEdit:focus {
            border: 2px solid #287ACB;
        }

        QPushButton {
            background: white;
            color: #173B6C;
            border: 1px solid #C7D7E9;
            border-radius: 6px;
            padding: 9px 15px;
            min-height: 20px;
        }

        QPushButton:hover {
            background: #EEF6FF;
            border-color: #287ACB;
        }

        #primaryButton {
            background: #246FD0;
            color: white;
            border: none;
            font-weight: bold;
        }

        #primaryButton:hover {
            background: #195FB9;
        }

        #deleteButton {
            color: #E53935;
        }

        #sideButton:hover {
            background: #24589A;
            border: none;
        }

        QTableWidget {
            background: white;
            border: 1px solid #D5E1EF;
            gridline-color: #E0E7EF;
            selection-background-color: #DCEEFF;
            selection-color: #173B6C;
            alternate-background-color: #F8FAFD;
        }

        QHeaderView::section {
            background: #EDF3F9;
            color: #173B6C;
            font-weight: bold;
            border: none;
            border-right: 1px solid #DCE5EF;
            border-bottom: 1px solid #DCE5EF;
            padding: 9px;
        }

        QTableWidget::item {
            padding: 6px;
        }
    )";

    setStyleSheet(style);
}


// ============================================================
// DONNEES DE DEMONSTRATION
// ============================================================

void Technicienne::chargerDonneesDemo()
{
    struct Demo
    {
        int id;
        const char *nom;
        const char *prenom;
        const char *email;
        const char *tel;
        const char *formation;
        int exp;
        const char *specialite;
        const char *competences;
        int statut;          // 0 = Actif, 1 = Inactif
        int annee, mois, jour;
    };

    const Demo demo[] = {
        { 1, "Ben Ali", "Sami", "sami.benali@smartweather.tn", "20123456",
         "Licence en électronique", 5, "Capteurs",
         "Étalonnage, maintenance préventive", 0, 2021, 3, 15 },

        { 2, "Trabelsi", "Amira", "amira.trabelsi@smartweather.tn", "98234567",
         "Master en télécommunications", 8, "Réseaux",
         "Transmission de données, configuration", 0, 2018, 9, 1 },

        { 3, "Gharbi", "Walid", "walid.gharbi@smartweather.tn", "55345678",
         "Technicien supérieur", 3, "Électricité",
         "Câblage, alimentation solaire", 1, 2022, 1, 20 },

        { 4, "Mansouri", "Rim", "rim.mansouri@smartweather.tn", "22456789",
         "Ingénieur en mécatronique", 6, "Automatisme",
         "Diagnostic, programmation", 0, 2020, 6, 10 }
    };

    const int n = static_cast<int>(sizeof(demo) / sizeof(demo[0]));

    for (int i = 0; i < n; ++i)
    {
        const Demo &d = demo[i];

        idTechnicien->setText(QString::number(d.id));
        nom->setText(QString::fromUtf8(d.nom));
        prenom->setText(QString::fromUtf8(d.prenom));
        email->setText(QString::fromUtf8(d.email));
        telephone->setText(QString::fromUtf8(d.tel));
        formation->setText(QString::fromUtf8(d.formation));
        experience->setValue(d.exp);
        specialite->setText(QString::fromUtf8(d.specialite));
        competences->setPlainText(QString::fromUtf8(d.competences));
        statut->setCurrentIndex(d.statut);
        dateIntegration->setDate(QDate(d.annee, d.mois, d.jour));

        const int row = table->rowCount();
        table->insertRow(row);
        remplirLigne(row);
    }

    viderChamps();
}


// ============================================================
// UTILITAIRES
// ============================================================

int Technicienne::trouverLigneParId(int id) const
{
    for (int r = 0; r < table->rowCount(); ++r)
    {
        const QTableWidgetItem *it = table->item(r, COL_ID);

        if (it && it->data(Qt::DisplayRole).toInt() == id)
            return r;
    }

    return -1;
}


bool Technicienne::validerChamps(int ligneIgnoree)
{
    bool ok = false;
    const int id = idTechnicien->text().trimmed().toInt(&ok);

    if (!ok || id <= 0)
    {
        QMessageBox::warning(this, "Erreur",
                             "L'ID doit être un nombre entier positif.");
        idTechnicien->setFocus();
        return false;
    }

    const int existante = trouverLigneParId(id);

    if (existante != -1 && existante != ligneIgnoree)
    {
        QMessageBox::warning(this, "Erreur",
                             "Cet ID existe déjà. Veuillez en choisir un autre.");
        idTechnicien->setFocus();
        return false;
    }

    if (nom->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Erreur", "Le nom est obligatoire.");
        nom->setFocus();
        return false;
    }

    if (prenom->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Erreur", "Le prénom est obligatoire.");
        prenom->setFocus();
        return false;
    }

    const QRegularExpression emailRegex(
        "^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");

    if (!emailRegex.match(email->text().trimmed()).hasMatch())
    {
        QMessageBox::warning(this, "Erreur", "L'adresse email n'est pas valide.");
        email->setFocus();
        return false;
    }

    const QRegularExpression telRegex("^[0-9]{8}$");

    if (!telRegex.match(telephone->text().trimmed()).hasMatch())
    {
        QMessageBox::warning(this, "Erreur",
                             "Le téléphone doit contenir exactement 8 chiffres.");
        telephone->setFocus();
        return false;
    }

    if (formation->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Erreur", "La formation est obligatoire.");
        formation->setFocus();
        return false;
    }

    if (specialite->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Erreur", "La spécialité est obligatoire.");
        specialite->setFocus();
        return false;
    }

    return true;
}


void Technicienne::remplirLigne(int row)
{
    // ID et expérience stockés comme nombres (tri numérique correct)
    QTableWidgetItem *itemId = new QTableWidgetItem;
    itemId->setData(Qt::DisplayRole, idTechnicien->text().trimmed().toInt());

    QTableWidgetItem *itemExp = new QTableWidgetItem;
    itemExp->setData(Qt::DisplayRole, experience->value());

    // La date est stockée comme QDate (tri chronologique correct)
    QTableWidgetItem *itemDate = new QTableWidgetItem;
    itemDate->setData(Qt::DisplayRole, dateIntegration->date());

    QTableWidgetItem *itemStatut = new QTableWidgetItem(statut->currentText());
    itemStatut->setForeground(
        statut->currentText() == "Actif" ? QColor("#16A765") : QColor("#EF4444"));

    table->setItem(row, COL_ID, itemId);
    table->setItem(row, COL_NOM, new QTableWidgetItem(nom->text().trimmed()));
    table->setItem(row, COL_PRENOM, new QTableWidgetItem(prenom->text().trimmed()));
    table->setItem(row, COL_EMAIL, new QTableWidgetItem(email->text().trimmed()));
    table->setItem(row, COL_TELEPHONE, new QTableWidgetItem(telephone->text().trimmed()));
    table->setItem(row, COL_FORMATION, new QTableWidgetItem(formation->text().trimmed()));
    table->setItem(row, COL_EXPERIENCE, itemExp);
    table->setItem(row, COL_SPECIALITE, new QTableWidgetItem(specialite->text().trimmed()));
    table->setItem(row, COL_COMPETENCES,
                   new QTableWidgetItem(
                       competences->toPlainText().simplified()));
    table->setItem(row, COL_STATUT, itemStatut);
    table->setItem(row, COL_DATE, itemDate);
}


// ============================================================
// ACTIONS CRUD
// ============================================================

void Technicienne::ajouterTechnicien()
{
    if (!validerChamps(-1))
        return;

    const int row = table->rowCount();
    table->insertRow(row);
    remplirLigne(row);

    updateStatistics();
    viderChamps();

    QMessageBox::information(this, "Succès",
                             "Le technicien a été ajouté avec succès.");
}


void Technicienne::modifierTechnicien()
{
    const int row = table->currentRow();

    if (row < 0)
    {
        QMessageBox::warning(this, "Modification",
                             "Veuillez sélectionner un technicien dans la liste.");
        return;
    }

    if (!validerChamps(row))
        return;

    remplirLigne(row);
    updateStatistics();

    QMessageBox::information(this, "Succès",
                             "Le technicien a été modifié avec succès.");
}


void Technicienne::supprimerTechnicien()
{
    const int row = table->currentRow();

    if (row < 0)
    {
        QMessageBox::warning(this, "Suppression",
                             "Veuillez sélectionner un technicien dans la liste.");
        return;
    }

    const QMessageBox::StandardButton reponse = QMessageBox::question(
        this,
        "Confirmation",
        "Voulez-vous vraiment supprimer ce technicien ?",
        QMessageBox::Yes | QMessageBox::No);

    if (reponse != QMessageBox::Yes)
        return;

    table->removeRow(row);

    updateStatistics();
    viderChamps();
}


void Technicienne::viderChamps()
{
    idTechnicien->clear();
    nom->clear();
    prenom->clear();
    email->clear();
    telephone->clear();
    formation->clear();
    specialite->clear();
    competences->clear();

    experience->setValue(0);
    statut->setCurrentIndex(0);
    dateIntegration->setDate(QDate::currentDate());

    table->clearSelection();
    table->setCurrentCell(-1, -1);
}


void Technicienne::selectionnerTechnicien(int row, int column)
{
    Q_UNUSED(column);

    if (row < 0 || row >= table->rowCount())
        return;

    const QTableWidgetItem *itemId = table->item(row, COL_ID);
    if (!itemId)
        return;

    idTechnicien->setText(QString::number(itemId->data(Qt::DisplayRole).toInt()));
    nom->setText(texteCellule(table, row, COL_NOM));
    prenom->setText(texteCellule(table, row, COL_PRENOM));
    email->setText(texteCellule(table, row, COL_EMAIL));
    telephone->setText(texteCellule(table, row, COL_TELEPHONE));
    formation->setText(texteCellule(table, row, COL_FORMATION));
    specialite->setText(texteCellule(table, row, COL_SPECIALITE));
    competences->setPlainText(texteCellule(table, row, COL_COMPETENCES));

    const QTableWidgetItem *itemExp = table->item(row, COL_EXPERIENCE);
    if (itemExp)
        experience->setValue(itemExp->data(Qt::DisplayRole).toInt());

    const int indexStatut = statut->findText(texteCellule(table, row, COL_STATUT));
    statut->setCurrentIndex(indexStatut >= 0 ? indexStatut : 0);

    const QTableWidgetItem *itemDate = table->item(row, COL_DATE);
    if (itemDate)
        dateIntegration->setDate(itemDate->data(Qt::DisplayRole).toDate());
}


// ============================================================
// RECHERCHE / TRI / EXPORT PDF
// ============================================================

void Technicienne::rechercherTechnicien()
{
    const QString critere = recherche->text().trimmed();

    int trouves = 0;

    for (int r = 0; r < table->rowCount(); ++r)
    {
        bool visible = critere.isEmpty();

        if (!visible)
        {
            for (int c = 0; c < table->columnCount(); ++c)
            {
                if (texteCellule(table, r, c).contains(critere, Qt::CaseInsensitive))
                {
                    visible = true;
                    break;
                }
            }
        }

        table->setRowHidden(r, !visible);

        if (visible)
            ++trouves;
    }

    if (!critere.isEmpty() && trouves == 0)
    {
        QMessageBox::information(this, "Recherche",
                                 "Aucun technicien ne correspond à votre recherche.");
    }
}


void Technicienne::trierTechnicien()
{
    int colonne = COL_ID;

    switch (triCombo->currentIndex())
    {
    case 0: colonne = COL_ID;         break;
    case 1: colonne = COL_NOM;        break;
    case 2: colonne = COL_PRENOM;     break;
    case 3: colonne = COL_EXPERIENCE; break;
    case 4: colonne = COL_STATUT;     break;
    case 5: colonne = COL_DATE;       break;
    default: break;
    }

    table->sortItems(colonne, Qt::AscendingOrder);
}


void Technicienne::exporterPDF()
{
    if (table->rowCount() == 0)
    {
        QMessageBox::information(this, "Export PDF",
                                 "Aucun technicien à exporter.");
        return;
    }

    QString chemin = QFileDialog::getSaveFileName(
        this,
        "Exporter la liste des techniciens",
        "techniciens.pdf",
        "Fichiers PDF (*.pdf)");

    if (chemin.isEmpty())
        return;

    if (!chemin.endsWith(".pdf", Qt::CaseInsensitive))
        chemin += ".pdf";

    QString html;
    html += "<h2 style='color:#174481;'>Liste des techniciens</h2>";
    html += "<p>Date : " + QDate::currentDate().toString("dd/MM/yyyy") + "</p>";
    html += "<table border='1' cellspacing='0' cellpadding='4' width='100%'>";
    html += "<tr style='background-color:#EDF3F9;'>";

    const int colonnesPdf[] = { COL_ID, COL_NOM, COL_PRENOM, COL_EMAIL,
                               COL_TELEPHONE, COL_SPECIALITE, COL_EXPERIENCE,
                               COL_STATUT, COL_DATE };
    const int nbColonnesPdf = static_cast<int>(sizeof(colonnesPdf) / sizeof(colonnesPdf[0]));

    for (int i = 0; i < nbColonnesPdf; ++i)
    {
        const QTableWidgetItem *entete =
            table->horizontalHeaderItem(colonnesPdf[i]);

        html += "<th>" + (entete ? entete->text().toHtmlEscaped() : QString()) + "</th>";
    }

    html += "</tr>";

    for (int r = 0; r < table->rowCount(); ++r)
    {
        if (table->isRowHidden(r))
            continue;

        html += "<tr>";

        for (int i = 0; i < nbColonnesPdf; ++i)
            html += "<td>" + texteCellule(table, r, colonnesPdf[i]).toHtmlEscaped() + "</td>";

        html += "</tr>";
    }

    html += "</table>";

    QTextDocument document;
    document.setHtml(html);

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(chemin);

    document.print(&printer);

    QMessageBox::information(this, "Export PDF",
                             "Le fichier PDF a été créé avec succès :\n" + chemin);
}


// ============================================================
// STATISTIQUES
// ============================================================

void Technicienne::updateStatistics()
{
    int actifs = 0;
    int inactifs = 0;

    for (int r = 0; r < table->rowCount(); ++r)
    {
        if (texteCellule(table, r, COL_STATUT) == "Actif")
            ++actifs;
        else
            ++inactifs;
    }

    lblTotal->setText(QString("Total : %1").arg(actifs + inactifs));
    lblActifs->setText(QString("Actifs : %1").arg(actifs));
    lblInactifs->setText(QString("Inactifs : %1").arg(inactifs));

    statisticsWidget->setStatistics(actifs, inactifs);
}
