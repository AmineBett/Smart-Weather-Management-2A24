#include "stationwindow.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QFontMetrics>
#include <QVector>

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

#include <QHeaderView>
#include <QAbstractItemView>
#include <QTableWidgetItem>

#include <QDate>
#include <QMap>
#include <algorithm>


// ============================================================
// STATISTICS WIDGET
// ============================================================

StatisticsWidget::StatisticsWidget(QWidget *parent)
    : QWidget(parent),
    m_actifs(0),
    m_maintenance(0),
    m_horsService(0)
{
    setMinimumHeight(170);
}


void StatisticsWidget::setStatistics(int actifs,
                                     int maintenance,
                                     int horsService)
{
    m_actifs = actifs;
    m_maintenance = maintenance;
    m_horsService = horsService;

    update();
}


void StatisticsWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const int total = m_actifs + m_maintenance + m_horsService;

    // ========================================================
    // Géométrie : [ donut ]  [ légende ]  centré dans le widget
    // ========================================================

    const QFont legendFont("Arial", 9);
    const QFontMetrics fm(legendFont);

    struct Item { QColor color; QString text; };
    QVector<Item> items;

    if (total == 0)
    {
        items.append({ QColor("#DCE8F5"), "Aucune station" });
    }
    else
    {
        auto pct = [total](int n) {
            return static_cast<double>(n) / total * 100.0;
        };

        items.append({ QColor("#16A765"),
                      QString("Actifs : %1 (%2%)")
                          .arg(m_actifs).arg(pct(m_actifs), 0, 'f', 1) });

        items.append({ QColor("#FF9F43"),
                      QString("Maintenance : %1 (%2%)")
                          .arg(m_maintenance).arg(pct(m_maintenance), 0, 'f', 1) });

        items.append({ QColor("#EF4444"),
                      QString("Hors service : %1 (%2%)")
                          .arg(m_horsService).arg(pct(m_horsService), 0, 'f', 1) });
    }

    int textWidth = 0;
    for (const Item &it : items)
        textWidth = std::max(textWidth, fm.horizontalAdvance(it.text));

    const int dot = 10;          // diamètre de la pastille
    const int dotGap = 8;        // pastille -> texte
    const int gap = 24;          // donut -> légende
    const int legendWidth = dot + dotGap + textWidth;

    // Diamètre du donut : s'adapte à la hauteur et à la largeur disponibles
    int diameter = std::min(height() - 10, 150);
    diameter = std::min(diameter, width() - legendWidth - gap - 10);
    diameter = std::max(diameter, 60);

    const int blockWidth = diameter + gap + legendWidth;
    const double x0 = (width() - blockWidth) / 2.0;
    const double y0 = (height() - diameter) / 2.0;

    const QRectF pieRect(x0, y0, diameter, diameter);
    const double holeSize = diameter * 0.62;
    const QRectF holeRect(
        pieRect.center().x() - holeSize / 2.0,
        pieRect.center().y() - holeSize / 2.0,
        holeSize,
        holeSize
        );

    // ========================================================
    // Diagramme circulaire
    // ========================================================

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

        const int maintenanceAngle =
            qRound(static_cast<double>(m_maintenance) / total * 360 * 16);

        const int horsServiceAngle =
            360 * 16 - actifAngle - maintenanceAngle;

        painter.setPen(Qt::NoPen);

        painter.setBrush(QColor("#16A765"));
        painter.drawPie(pieRect, startAngle, -actifAngle);

        painter.setBrush(QColor("#FF9F43"));
        painter.drawPie(pieRect, startAngle - actifAngle, -maintenanceAngle);

        painter.setBrush(QColor("#EF4444"));
        painter.drawPie(pieRect,
                        startAngle - actifAngle - maintenanceAngle,
                        -horsServiceAngle);

        // Trou central
        painter.setBrush(Qt::white);
        painter.drawEllipse(holeRect);
    }

    // ========================================================
    // Texte au centre du donut
    // ========================================================

    painter.setPen(QColor("#173B6C"));

    QFont numberFont("Arial", 22, QFont::Bold);
    painter.setFont(numberFont);

    painter.drawText(
        QRectF(holeRect.left(), holeRect.center().y() - 28, holeRect.width(), 32),
        Qt::AlignCenter,
        QString::number(total)
        );

    painter.setFont(QFont("Arial", 9));

    painter.drawText(
        QRectF(holeRect.left(), holeRect.center().y() + 4, holeRect.width(), 20),
        Qt::AlignCenter,
        "stations"
        );

    // ========================================================
    // Légende à droite du donut
    // ========================================================

    painter.setFont(legendFont);

    const int rowHeight = 28;
    const double legendX = x0 + diameter + gap;
    double rowY = height() / 2.0 - (items.size() * rowHeight) / 2.0;

    for (const Item &it : items)
    {
        const double cy = rowY + rowHeight / 2.0;

        painter.setPen(Qt::NoPen);
        painter.setBrush(it.color);
        painter.drawEllipse(QRectF(legendX, cy - dot / 2.0, dot, dot));

        painter.setPen(QColor("#234A78"));
        painter.drawText(
            QRectF(legendX + dot + dotGap, rowY, textWidth + 4, rowHeight),
            Qt::AlignVCenter | Qt::AlignLeft,
            it.text
            );

        rowY += rowHeight;
    }
}


// ============================================================
// STATION WINDOW
// ============================================================

StationWindow::StationWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(
        "SmartWeather - Gestion des stations"
        );

    resize(1450, 850);

    setMinimumSize(1200, 700);

    setupUI();

    setupStyle();

    loadDemoData();

    refreshTable();

    updateStatistics();

    updateLocationList();
}


// ============================================================
// SETUP UI
// ============================================================

void StationWindow::setupUI()
{
    centralWidget = new QWidget(this);

    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout =
        new QHBoxLayout(centralWidget);

    mainLayout->setContentsMargins(
        0,
        0,
        0,
        0
        );

    mainLayout->setSpacing(0);


    // ========================================================
    // SIDEBAR
    // ========================================================

    QWidget *sidebar = createSidebar();

    sidebar->setFixedWidth(235);

    mainLayout->addWidget(sidebar);


    // ========================================================
    // PARTIE DROITE
    // ========================================================

    QWidget *content = new QWidget;

    QVBoxLayout *contentLayout =
        new QVBoxLayout(content);

    contentLayout->setContentsMargins(
        20,
        0,
        20,
        15
        );

    contentLayout->setSpacing(12);


    // Header
    contentLayout->addWidget(
        createHeader()
        );


    // Formulaire
    contentLayout->addWidget(
        createStationForm()
        );


    // Barre d'actions
    contentLayout->addWidget(
        createActionBar()
        );


    // ========================================================
    // PARTIE CENTRALE
    // ========================================================

    QHBoxLayout *bottomLayout =
        new QHBoxLayout;

    bottomLayout->setSpacing(12);


    // Liste
    QGroupBox *listBox =
        createStationList();

    bottomLayout->addWidget(
        listBox,
        7
        );


    // Colonne droite
    QVBoxLayout *rightColumn =
        new QVBoxLayout;

    rightColumn->setSpacing(12);


    QGroupBox *statsBox =
        createStatistics();

    rightColumn->addWidget(
        statsBox,
        5
        );


    QGroupBox *locationBox =
        createLocationBox();

    rightColumn->addWidget(
        locationBox,
        4
        );


    bottomLayout->addLayout(
        rightColumn,
        3
        );


    contentLayout->addLayout(
        bottomLayout,
        1
        );


    mainLayout->addWidget(
        content,
        1
        );
}


// ============================================================
// SIDEBAR
// ============================================================

QWidget *StationWindow::createSidebar()
{
    QWidget *sidebar =
        new QWidget;

    sidebar->setObjectName(
        "sidebar"
        );


    QVBoxLayout *layout =
        new QVBoxLayout(sidebar);

    layout->setContentsMargins(
        10,
        25,
        10,
        20
        );

    layout->setSpacing(8);


    // Logo soleil
    QLabel *logo =
        new QLabel("☀");

    logo->setAlignment(
        Qt::AlignCenter
        );

    logo->setStyleSheet(
        "font-size: 48px;"
        "color: #FFD21F;"
        );

    layout->addWidget(logo);


    // Nom
    QLabel *title =
        new QLabel(
            "SMART WEATHER\nMANAGEMENT"
            );

    title->setAlignment(
        Qt::AlignCenter
        );

    title->setStyleSheet(
        "color: white;"
        "font-size: 18px;"
        "font-weight: bold;"
        "padding-bottom: 30px;"
        );

    layout->addWidget(title);


    // Accueil
    QPushButton *home =
        new QPushButton(
            "⌂   Accueil"
            );

    home->setObjectName(
        "sideButton"
        );

    layout->addWidget(home);


    // Gestion stations
    QPushButton *stationsButton =
        new QPushButton(
            "☁   Gestion des stations"
            );

    stationsButton->setObjectName(
        "sideButtonActive"
        );

    layout->addWidget(
        stationsButton
        );


    // Équipements
    QPushButton *equipment =
        new QPushButton(
            "⚙   Gestion des équipements"
            );

    equipment->setObjectName(
        "sideButton"
        );

    layout->addWidget(
        equipment
        );


    // Techniciens
    QPushButton *technicians =
        new QPushButton(
            "♟   Gestion des techniciens"
            );

    technicians->setObjectName(
        "sideButton"
        );

    layout->addWidget(
        technicians
        );


    // Utilisateurs
    QPushButton *users =
        new QPushButton(
            "♟   Gestion des utilisateurs"
            );

    users->setObjectName(
        "sideButton"
        );

    layout->addWidget(
        users
        );


    layout->addStretch();


    QPushButton *settings =
        new QPushButton(
            "⚙   Paramètres"
            );

    settings->setObjectName(
        "sideButton"
        );

    layout->addWidget(
        settings
        );


    return sidebar;
}


// ============================================================
// HEADER
// ============================================================

QWidget *StationWindow::createHeader()
{
    QWidget *header =
        new QWidget;

    header->setObjectName(
        "header"
        );

    header->setFixedHeight(82);


    QHBoxLayout *layout =
        new QHBoxLayout(header);

    layout->setContentsMargins(
        25,
        8,
        25,
        8
        );


    QVBoxLayout *titles =
        new QVBoxLayout;

    QLabel *title =
        new QLabel(
            "Gestion des stations"
            );

    title->setObjectName(
        "headerTitle"
        );


    QLabel *subtitle =
        new QLabel(
            "Gérez les stations météorologiques et leurs informations"
            );

    subtitle->setObjectName(
        "headerSubtitle"
        );


    titles->addWidget(title);

    titles->addWidget(subtitle);


    layout->addLayout(
        titles
        );

    layout->addStretch();


    QLabel *date =
        new QLabel(
            QDate::currentDate()
                .toString("dd/MM/yyyy")
            );

    date->setStyleSheet(
        "color: white;"
        "font-weight: bold;"
        "font-size: 14px;"
        );


    QLabel *separator =
        new QLabel("  •  ");

    separator->setStyleSheet(
        "color: white;"
        );


    QLabel *admin =
        new QLabel(
            "Administrateur ▼"
            );

    admin->setStyleSheet(
        "color: white;"
        "font-weight: bold;"
        );


    layout->addWidget(date);

    layout->addWidget(separator);

    layout->addWidget(admin);


    return header;
}


// ============================================================
// FORMULAIRE STATION
// ============================================================

QGroupBox *StationWindow::createStationForm()
{
    QGroupBox *box =
        new QGroupBox(
            "☁   Informations de la station"
            );


    QGridLayout *layout =
        new QGridLayout(box);

    layout->setContentsMargins(
        20,
        25,
        20,
        18
        );

    layout->setHorizontalSpacing(12);

    layout->setVerticalSpacing(10);


    // ========================================================
    // ID
    // ========================================================

    QLabel *idLabel =
        new QLabel("ID Station :");

    idEdit =
        new QSpinBox;

    idEdit->setRange(
        1,
        999999
        );

    idEdit->setValue(1);


    // ========================================================
    // NOM
    // ========================================================

    QLabel *nomLabel =
        new QLabel("Nom :");

    nomEdit =
        new QLineEdit;

    nomEdit->setPlaceholderText(
        "Nom de la station"
        );


    // ========================================================
    // LOCALISATION
    // ========================================================

    QLabel *localisationLabel =
        new QLabel("Localisation :");

    localisationEdit =
        new QLineEdit;

    localisationEdit->setPlaceholderText(
        "Ville / localisation"
        );


    // ========================================================
    // LATITUDE
    // ========================================================

    QLabel *latitudeLabel =
        new QLabel("Latitude :");

    latitudeEdit =
        new QDoubleSpinBox;

    latitudeEdit->setRange(
        -90,
        90
        );

    latitudeEdit->setDecimals(4);

    latitudeEdit->setSingleStep(
        0.0001
        );


    // ========================================================
    // LONGITUDE
    // ========================================================

    QLabel *longitudeLabel =
        new QLabel("Longitude :");

    longitudeEdit =
        new QDoubleSpinBox;

    longitudeEdit->setRange(
        -180,
        180
        );

    longitudeEdit->setDecimals(4);

    longitudeEdit->setSingleStep(
        0.0001
        );


    // ========================================================
    // DATE
    // ========================================================

    QLabel *dateLabel =
        new QLabel(
            "Date d'installation :"
            );

    dateInstallationEdit =
        new QDateEdit;

    dateInstallationEdit->setCalendarPopup(
        true
        );

    dateInstallationEdit->setDate(
        QDate::currentDate()
        );

    dateInstallationEdit->setDisplayFormat(
        "dd/MM/yyyy"
        );


    // ========================================================
    // TYPE
    // ========================================================

    QLabel *typeLabel =
        new QLabel(
            "Type de station :"
            );

    typeCombo =
        new QComboBox;

    typeCombo->addItems(
        QStringList()
        << "Automatique"
        << "Manuelle"
        << "Hybride"
        );


    // ========================================================
    // ETAT
    // ========================================================

    QLabel *etatLabel =
        new QLabel(
            "Statut :"
            );

    etatCombo =
        new QComboBox;

    etatCombo->addItems(
        QStringList()
        << "Actif"
        << "Maintenance"
        << "Hors service"
        );


    // ========================================================
    // Placement
    // ========================================================

    layout->addWidget(
        idLabel,
        0,
        0
        );

    layout->addWidget(
        idEdit,
        0,
        1
        );


    layout->addWidget(
        nomLabel,
        0,
        2
        );

    layout->addWidget(
        nomEdit,
        0,
        3
        );


    layout->addWidget(
        localisationLabel,
        1,
        0
        );

    layout->addWidget(
        localisationEdit,
        1,
        1
        );


    layout->addWidget(
        latitudeLabel,
        1,
        2
        );

    layout->addWidget(
        latitudeEdit,
        1,
        3
        );


    layout->addWidget(
        longitudeLabel,
        2,
        0
        );

    layout->addWidget(
        longitudeEdit,
        2,
        1
        );


    layout->addWidget(
        dateLabel,
        2,
        2
        );

    layout->addWidget(
        dateInstallationEdit,
        2,
        3
        );


    layout->addWidget(
        typeLabel,
        3,
        0
        );

    layout->addWidget(
        typeCombo,
        3,
        1
        );


    layout->addWidget(
        etatLabel,
        3,
        2
        );

    layout->addWidget(
        etatCombo,
        3,
        3
        );


    layout->setColumnStretch(
        1,
        1
        );

    layout->setColumnStretch(
        3,
        2
        );


    return box;
}


// ============================================================
// BARRE ACTIONS
// ============================================================

QGroupBox *StationWindow::createActionBar()
{
    QGroupBox *box =
        new QGroupBox;

    QHBoxLayout *layout =
        new QHBoxLayout(box);

    layout->setContentsMargins(
        12,
        10,
        12,
        10
        );


    QPushButton *addButton =
        new QPushButton(
            "＋  Ajouter"
            );

    addButton->setObjectName(
        "primaryButton"
        );


    QPushButton *modifyButton =
        new QPushButton(
            "✎  Modifier"
            );


    QPushButton *deleteButton =
        new QPushButton(
            "▣  Supprimer"
            );

    deleteButton->setObjectName(
        "deleteButton"
        );


    QPushButton *clearButton =
        new QPushButton(
            "↻  Vider"
            );


    layout->addWidget(
        addButton
        );

    layout->addWidget(
        modifyButton
        );

    layout->addWidget(
        deleteButton
        );

    layout->addWidget(
        clearButton
        );


    layout->addSpacing(20);


    QLabel *sortLabel =
        new QLabel(
            "Trier par"
            );

    sortCombo =
        new QComboBox;

    sortCombo->addItems(
        QStringList()
        << "Nom"
        << "ID"
        << "Localisation"
        << "Date d'installation"
        << "Statut"
        );

    sortCombo->setMinimumWidth(
        160
        );


    QPushButton *sortButton =
        new QPushButton(
            "Trier"
            );


    layout->addWidget(
        sortLabel
        );

    layout->addWidget(
        sortCombo
        );

    layout->addWidget(
        sortButton
        );


    layout->addStretch();


    searchEdit =
        new QLineEdit;

    searchEdit->setPlaceholderText(
        "Rechercher une station..."
        );

    searchEdit->setMinimumWidth(
        220
        );


    QPushButton *searchButton =
        new QPushButton(
            "🔍"
            );

    searchButton->setFixedWidth(
        50
        );


    QPushButton *exportButton =
        new QPushButton(
            "☷  Exporter"
            );


    layout->addWidget(
        searchEdit
        );

    layout->addWidget(
        searchButton
        );

    layout->addWidget(
        exportButton
        );


    // ========================================================
    // Connections
    // ========================================================

    connect(
        addButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            addStation();
        }
        );


    connect(
        modifyButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            modifyStation();
        }
        );


    connect(
        deleteButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            deleteStation();
        }
        );


    connect(
        clearButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            clearForm();
        }
        );


    connect(
        sortButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            sortStations();
        }
        );


    connect(
        searchButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            searchStations();
        }
        );


    connect(
        searchEdit,
        &QLineEdit::returnPressed,
        this,
        [this]()
        {
            searchStations();
        }
        );


    connect(
        exportButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            exportStations();
        }
        );


    return box;
}


// ============================================================
// LISTE STATIONS
// ============================================================

QGroupBox *StationWindow::createStationList()
{
    QGroupBox *box =
        new QGroupBox(
            "▤   Liste des stations"
            );


    QVBoxLayout *layout =
        new QVBoxLayout(box);

    layout->setContentsMargins(
        15,
        15,
        15,
        12
        );


    QLabel *instruction =
        new QLabel(
            "Sélectionnez une ligne pour modifier les informations"
            );

    instruction->setAlignment(
        Qt::AlignRight
        );

    instruction->setStyleSheet(
        "color: #7B8CA5;"
        "font-size: 11px;"
        );


    layout->addWidget(
        instruction
        );


    table =
        new QTableWidget;

    table->setColumnCount(8);

    table->setHorizontalHeaderLabels(
        QStringList()
        << "ID"
        << "Nom"
        << "Localisation"
        << "Latitude"
        << "Longitude"
        << "Date d'installation"
        << "Type"
        << "Statut"
        );


    table->setSelectionBehavior(
        QAbstractItemView::SelectRows
        );

    table->setSelectionMode(
        QAbstractItemView::SingleSelection
        );

    table->setEditTriggers(
        QAbstractItemView::NoEditTriggers
        );

    table->setAlternatingRowColors(
        true
        );

    table->verticalHeader()->setVisible(
        true
        );

    table->horizontalHeader()
        ->setStretchLastSection(true);

    table->horizontalHeader()
        ->setSectionResizeMode(
            QHeaderView::Interactive
            );

    table->setColumnWidth(0, 55);
    table->setColumnWidth(1, 145);
    table->setColumnWidth(2, 120);
    table->setColumnWidth(3, 85);
    table->setColumnWidth(4, 85);
    table->setColumnWidth(5, 125);
    table->setColumnWidth(6, 105);


    layout->addWidget(
        table
        );


    totalLabel =
        new QLabel(
            "0 station(s) trouvée(s)"
            );

    totalLabel->setStyleSheet(
        "font-weight: bold;"
        "color: #173B6C;"
        );


    layout->addWidget(
        totalLabel
        );


    connect(
        table,
        &QTableWidget::cellClicked,
        this,
        [this](int row, int)
        {
            Q_UNUSED(row);

            fillFormFromSelectedRow();
        }
        );


    return box;
}


// ============================================================
// STATISTIQUES
// ============================================================

QGroupBox *StationWindow::createStatistics()
{
    QGroupBox *box =
        new QGroupBox(
            "📊   Statistiques des stations"
            );


    QVBoxLayout *layout =
        new QVBoxLayout(box);

    layout->setContentsMargins(
        10,
        20,
        10,
        10
        );


    statisticsWidget =
        new StatisticsWidget;


    layout->addWidget(
        statisticsWidget
        );


    return box;
}


// ============================================================
// LOCALISATION
// ============================================================

QGroupBox *StationWindow::createLocationBox()
{
    QGroupBox *box =
        new QGroupBox(
            "📍   Localisation géographique – Tunisie"
            );


    QVBoxLayout *layout =
        new QVBoxLayout(box);

    layout->setContentsMargins(
        15,
        20,
        15,
        15
        );


    QLabel *locationTitle =
        new QLabel(
            "Stations installées"
            );

    locationTitle->setStyleSheet(
        "font-size: 14px;"
        "font-weight: bold;"
        "color: #173B6C;"
        );


    layout->addWidget(
        locationTitle
        );


    QLabel *locationList =
        new QLabel;

    locationList->setObjectName(
        "locationList"
        );

    locationList->setWordWrap(
        true
        );

    locationList->setAlignment(
        Qt::AlignTop
        );

    layout->addWidget(
        locationList
        );


    return box;
}


// ============================================================
// STYLE
// ============================================================

void StationWindow::setupStyle()
{
    QString style = R"(
        QMainWindow {
            background: #F4F7FB;
        }

        QWidget {
            font-family: Arial;
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
            padding: 12px;
            text-align: left;
            font-size: 13px;
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
            font-size: 25px;
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
            font-size: 15px;
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
        QDoubleSpinBox,
        QDateEdit,
        QComboBox {
            background: white;
            border: 1px solid #C9D8E9;
            border-radius: 6px;
            padding: 8px;
            min-height: 20px;
        }

        QLineEdit:focus,
        QSpinBox:focus,
        QDoubleSpinBox:focus,
        QDateEdit:focus,
        QComboBox:focus {
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

        #locationList {
            background: #EAF5FD;
            border-radius: 8px;
            padding: 15px;
            color: #315A83;
        }
    )";

    setStyleSheet(style);
}


// ============================================================
// DEMO DATA
// ============================================================

void StationWindow::loadDemoData()
{
    stations.clear();


    Station s1;

    s1.id = 1;
    s1.nom = "Station Tunis";
    s1.localisation = "Tunis";
    s1.latitude = 36.8065;
    s1.longitude = 10.1815;
    s1.dateInstallation = QDate(2025, 3, 12);
    s1.type = "Automatique";
    s1.etat = "Actif";

    stations.append(s1);


    Station s2;

    s2.id = 2;
    s2.nom = "Station Sfax";
    s2.localisation = "Sfax";
    s2.latitude = 34.7406;
    s2.longitude = 10.7603;
    s2.dateInstallation = QDate(2025, 5, 20);
    s2.type = "Automatique";
    s2.etat = "Actif";

    stations.append(s2);


    Station s3;

    s3.id = 3;
    s3.nom = "Station Sousse";
    s3.localisation = "Sousse";
    s3.latitude = 35.8256;
    s3.longitude = 10.6370;
    s3.dateInstallation = QDate(2024, 10, 10);
    s3.type = "Hybride";
    s3.etat = "Maintenance";

    stations.append(s3);


    Station s4;

    s4.id = 4;
    s4.nom = "Station Bizerte";
    s4.localisation = "Bizerte";
    s4.latitude = 37.2746;
    s4.longitude = 9.8739;
    s4.dateInstallation = QDate(2024, 7, 15);
    s4.type = "Automatique";
    s4.etat = "Actif";

    stations.append(s4);


    Station s5;

    s5.id = 5;
    s5.nom = "Station Kairouan";
    s5.localisation = "Kairouan";
    s5.latitude = 35.6781;
    s5.longitude = 10.0963;
    s5.dateInstallation = QDate(2023, 8, 9);
    s5.type = "Manuelle";
    s5.etat = "Hors service";

    stations.append(s5);


    Station s6;

    s6.id = 6;
    s6.nom = "Station Gabès";
    s6.localisation = "Gabès";
    s6.latitude = 33.8815;
    s6.longitude = 10.0982;
    s6.dateInstallation = QDate(2025, 1, 25);
    s6.type = "Automatique";
    s6.etat = "Actif";

    stations.append(s6);


    Station s7;

    s7.id = 7;
    s7.nom = "Station Gafsa";
    s7.localisation = "Gafsa";
    s7.latitude = 34.4250;
    s7.longitude = 8.7842;
    s7.dateInstallation = QDate(2024, 4, 18);
    s7.type = "Hybride";
    s7.etat = "Maintenance";

    stations.append(s7);


    Station s8;

    s8.id = 8;
    s8.nom = "Station Nabeul";
    s8.localisation = "Nabeul";
    s8.latitude = 36.4510;
    s8.longitude = 10.7350;
    s8.dateInstallation = QDate(2025, 2, 9);
    s8.type = "Automatique";
    s8.etat = "Actif";

    stations.append(s8);
}


// ============================================================
// REFRESH TABLE
// ============================================================

void StationWindow::refreshTable()
{
    QString search =
        searchEdit
            ? searchEdit->text().trimmed().toLower()
            : "";


    table->setRowCount(0);


    for (const Station &station : stations)
    {
        bool visible = true;


        if (!search.isEmpty())
        {
            QString data =
                QString::number(station.id)
                + " "
                + station.nom
                + " "
                + station.localisation
                + " "
                + station.type
                + " "
                + station.etat;

            if (!data.toLower().contains(search))
            {
                visible = false;
            }
        }


        if (!visible)
            continue;


        int row =
            table->rowCount();

        table->insertRow(row);


        table->setItem(
            row,
            0,
            new QTableWidgetItem(
                QString::number(station.id)
                )
            );


        table->setItem(
            row,
            1,
            new QTableWidgetItem(
                station.nom
                )
            );


        table->setItem(
            row,
            2,
            new QTableWidgetItem(
                station.localisation
                )
            );


        table->setItem(
            row,
            3,
            new QTableWidgetItem(
                QString::number(
                    station.latitude,
                    'f',
                    4
                    )
                )
            );


        table->setItem(
            row,
            4,
            new QTableWidgetItem(
                QString::number(
                    station.longitude,
                    'f',
                    4
                    )
                )
            );


        table->setItem(
            row,
            5,
            new QTableWidgetItem(
                station.dateInstallation
                    .toString("dd/MM/yyyy")
                )
            );


        table->setItem(
            row,
            6,
            new QTableWidgetItem(
                station.type
                )
            );


        QTableWidgetItem *statusItem =
            new QTableWidgetItem(
                station.etat
                );


        if (station.etat == "Actif")
        {
            statusItem->setForeground(
                QColor("#16A765")
                );
        }
        else if (station.etat == "Maintenance")
        {
            statusItem->setForeground(
                QColor("#F39C12")
                );
        }
        else
        {
            statusItem->setForeground(
                QColor("#E53935")
                );
        }


        statusItem->setFont(
            QFont(
                "Arial",
                10,
                QFont::Bold
                )
            );


        table->setItem(
            row,
            7,
            statusItem
            );
    }


    totalLabel->setText(
        QString("%1 station(s) trouvée(s)")
            .arg(table->rowCount())
        );
}


// ============================================================
// UPDATE STATISTICS
// ============================================================

void StationWindow::updateStatistics()
{
    int actifs = 0;
    int maintenance = 0;
    int horsService = 0;


    for (const Station &station : stations)
    {
        if (station.etat == "Actif")
        {
            actifs++;
        }
        else if (station.etat == "Maintenance")
        {
            maintenance++;
        }
        else if (station.etat == "Hors service")
        {
            horsService++;
        }
    }


    statisticsWidget->setStatistics(
        actifs,
        maintenance,
        horsService
        );
}


// ============================================================
// ADD
// ============================================================

void StationWindow::addStation()
{
    if (nomEdit->text().trimmed().isEmpty())
    {
        QMessageBox::warning(
            this,
            "SmartWeather",
            "Veuillez saisir le nom de la station."
            );

        return;
    }


    for (const Station &station : stations)
    {
        if (station.id == idEdit->value())
        {
            QMessageBox::warning(
                this,
                "SmartWeather",
                "Cet ID de station existe déjà."
                );

            return;
        }
    }


    Station station;

    station.id =
        idEdit->value();

    station.nom =
        nomEdit->text().trimmed();

    station.localisation =
        localisationEdit->text().trimmed();

    station.latitude =
        latitudeEdit->value();

    station.longitude =
        longitudeEdit->value();

    station.dateInstallation =
        dateInstallationEdit->date();

    station.type =
        typeCombo->currentText();

    station.etat =
        etatCombo->currentText();


    stations.append(
        station
        );


    refreshTable();

    updateStatistics();

    updateLocationList();

    clearForm();


    QMessageBox::information(
        this,
        "SmartWeather",
        "La station a été ajoutée avec succès."
        );
}


// ============================================================
// MODIFY
// ============================================================

void StationWindow::modifyStation()
{
    int row =
        selectedRow();


    if (row < 0)
    {
        QMessageBox::warning(
            this,
            "SmartWeather",
            "Veuillez sélectionner une station dans le tableau."
            );

        return;
    }


    int id =
        table->item(row, 0)
            ->text()
            .toInt();


    for (int i = 0;
         i < stations.size();
         ++i)
    {
        if (stations[i].id == id)
        {
            stations[i].id =
                idEdit->value();

            stations[i].nom =
                nomEdit->text().trimmed();

            stations[i].localisation =
                localisationEdit->text().trimmed();

            stations[i].latitude =
                latitudeEdit->value();

            stations[i].longitude =
                longitudeEdit->value();

            stations[i].dateInstallation =
                dateInstallationEdit->date();

            stations[i].type =
                typeCombo->currentText();

            stations[i].etat =
                etatCombo->currentText();

            break;
        }
    }


    refreshTable();

    updateStatistics();

    updateLocationList();


    QMessageBox::information(
        this,
        "SmartWeather",
        "La station a été modifiée."
        );
}


// ============================================================
// DELETE
// ============================================================

void StationWindow::deleteStation()
{
    int row =
        selectedRow();


    if (row < 0)
    {
        QMessageBox::warning(
            this,
            "SmartWeather",
            "Veuillez sélectionner une station."
            );

        return;
    }


    int id =
        table->item(row, 0)
            ->text()
            .toInt();


    QMessageBox::StandardButton answer =
        QMessageBox::question(
            this,
            "Supprimer",
            "Voulez-vous vraiment supprimer cette station ?",
            QMessageBox::Yes |
                QMessageBox::No
            );


    if (answer != QMessageBox::Yes)
        return;


    for (int i = 0;
         i < stations.size();
         ++i)
    {
        if (stations[i].id == id)
        {
            stations.remove(i);
            break;
        }
    }


    refreshTable();

    updateStatistics();

    updateLocationList();

    clearForm();
}


// ============================================================
// SEARCH
// ============================================================

void StationWindow::searchStations()
{
    refreshTable();
}


// ============================================================
// SORT
// ============================================================

void StationWindow::sortStations()
{
    QString criterion =
        sortCombo->currentText();


    if (criterion == "Nom")
    {
        std::sort(
            stations.begin(),
            stations.end(),
            [](const Station &a,
               const Station &b)
            {
                return a.nom.toLower()
                < b.nom.toLower();
            }
            );
    }
    else if (criterion == "ID")
    {
        std::sort(
            stations.begin(),
            stations.end(),
            [](const Station &a,
               const Station &b)
            {
                return a.id < b.id;
            }
            );
    }
    else if (criterion == "Localisation")
    {
        std::sort(
            stations.begin(),
            stations.end(),
            [](const Station &a,
               const Station &b)
            {
                return a.localisation.toLower()
                < b.localisation.toLower();
            }
            );
    }
    else if (criterion == "Date d'installation")
    {
        std::sort(
            stations.begin(),
            stations.end(),
            [](const Station &a,
               const Station &b)
            {
                return a.dateInstallation
                       < b.dateInstallation;
            }
            );
    }
    else if (criterion == "Statut")
    {
        std::sort(
            stations.begin(),
            stations.end(),
            [](const Station &a,
               const Station &b)
            {
                return a.etat.toLower()
                < b.etat.toLower();
            }
            );
    }


    refreshTable();
}


// ============================================================
// CLEAR FORM
// ============================================================

void StationWindow::clearForm()
{
    int nextId = 1;


    for (const Station &station : stations)
    {
        if (station.id >= nextId)
        {
            nextId =
                station.id + 1;
        }
    }


    idEdit->setValue(
        nextId
        );

    nomEdit->clear();

    localisationEdit->clear();

    latitudeEdit->setValue(
        36.8065
        );

    longitudeEdit->setValue(
        10.1815
        );

    dateInstallationEdit->setDate(
        QDate::currentDate()
        );

    typeCombo->setCurrentIndex(
        0
        );

    etatCombo->setCurrentIndex(
        0
        );


    table->clearSelection();
}


// ============================================================
// SELECTED ROW
// ============================================================

int StationWindow::selectedRow() const
{
    QList<QTableWidgetSelectionRange> ranges =
        table->selectedRanges();


    if (ranges.isEmpty())
        return -1;


    return ranges.first().topRow();
}


// ============================================================
// FILL FORM
// ============================================================

void StationWindow::fillFormFromSelectedRow()
{
    int row =
        selectedRow();


    if (row < 0)
        return;


    idEdit->setValue(
        table->item(row, 0)
            ->text()
            .toInt()
        );


    nomEdit->setText(
        table->item(row, 1)
            ->text()
        );


    localisationEdit->setText(
        table->item(row, 2)
            ->text()
        );


    latitudeEdit->setValue(
        table->item(row, 3)
            ->text()
            .toDouble()
        );


    longitudeEdit->setValue(
        table->item(row, 4)
            ->text()
            .toDouble()
        );


    dateInstallationEdit->setDate(
        QDate::fromString(
            table->item(row, 5)
                ->text(),
            "dd/MM/yyyy"
            )
        );


    typeCombo->setCurrentText(
        table->item(row, 6)
            ->text()
        );


    etatCombo->setCurrentText(
        table->item(row, 7)
            ->text()
        );
}


// ============================================================
// EXPORT CSV
// ============================================================

void StationWindow::exportStations()
{
    QString fileName =
        QFileDialog::getSaveFileName(
            this,
            "Exporter les stations",
            "stations.csv",
            "Fichier CSV (*.csv)"
            );


    if (fileName.isEmpty())
        return;


    QFile file(fileName);


    if (!file.open(QIODevice::WriteOnly |
                   QIODevice::Text))
    {
        QMessageBox::warning(
            this,
            "SmartWeather",
            "Impossible de créer le fichier."
            );

        return;
    }


    QTextStream stream(
        &file
        );


    // Header CSV
    stream
        << "ID;"
        << "Nom;"
        << "Localisation;"
        << "Latitude;"
        << "Longitude;"
        << "Date installation;"
        << "Type;"
        << "Statut\n";


    for (const Station &station : stations)
    {
        stream
            << station.id << ";"
            << station.nom << ";"
            << station.localisation << ";"
            << station.latitude << ";"
            << station.longitude << ";"
            << station.dateInstallation
                   .toString("dd/MM/yyyy")
            << ";"
            << station.type << ";"
            << station.etat
            << "\n";
    }


    file.close();


    QMessageBox::information(
        this,
        "SmartWeather",
        "Les stations ont été exportées avec succès."
        );
}


// ============================================================
// LOCATION LIST
// ============================================================

void StationWindow::updateLocationList()
{
    QLabel *label =
        findChild<QLabel *>(
            "locationList"
            );


    if (!label)
        return;


    QString text;


    for (const Station &station : stations)
    {
        QString symbol;


        if (station.etat == "Actif")
        {
            symbol = "🟢";
        }
        else if (station.etat == "Maintenance")
        {
            symbol = "🟠";
        }
        else
        {
            symbol = "🔴";
        }


        text +=
            symbol
            + "  "
            + station.nom
            + " — "
            + station.localisation
            + "\n";


        text +=
            "    "
            + QString::number(
                station.latitude,
                'f',
                4
                )
            + " / "
            + QString::number(
                station.longitude,
                'f',
                4
                )
            + "\n\n";
    }


    if (text.isEmpty())
    {
        text =
            "Aucune station enregistrée.";
    }


    label->setText(
        text
        );
}