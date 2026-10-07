#include "weatherwisetest.h"

#include <QAbstractItemModel>
#include <QComboBox>
#include <QDateTime>
#include <QDateEdit>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPageLayout>
#include <QPageSize>
#include <QPdfWriter>
#include <QPushButton>
#include <QTableWidget>
#include <QTextDocument>
#include <QTimeZone>
#include <QTimer>
#include <QVBoxLayout>

#include <algorithm>

/* ------------------------------------------------------------------ */
/* Pays utilisé                                                        */
/* ------------------------------------------------------------------ */

static const QLocale::Territory kForcedCountry =
    QLocale::Tunisia;

/* ------------------------------------------------------------------ */
/* Données géographiques                                               */
/* ------------------------------------------------------------------ */

struct CityRow
{
    const char *n;
    double lat;
    double lon;
};

#define COUNT(a) int(sizeof(a) / sizeof(a[0]))

/* Tunisie */

static const double TN_OUT[][2] =
    {
        {37.00, 8.60},
        {37.35, 9.80},
        {37.10, 11.00},
        {36.40, 10.60},
        {35.70, 10.90},
        {35.20, 11.10},
        {34.30, 10.30},
        {33.90, 10.10},
        {33.30, 11.20},
        {33.10, 11.50},
        {32.40, 11.20},
        {31.70, 10.30},
        {30.30, 9.50},
        {32.00, 9.00},
        {32.90, 8.30},
        {33.90, 7.50},
        {34.50, 8.30},
        {35.90, 8.30},
        {36.90, 8.40}
};

static const CityRow TN_CITIES[] =
    {
        {"Tunis", 36.80, 10.18},
        {"Sfax", 34.74, 10.76},
        {"Sousse", 35.83, 10.64},
        {"Kairouan", 35.68, 10.10},
        {"Gabès", 33.88, 10.10},
        {"Tozeur", 33.92, 8.13},
        {"Kasserine", 35.17, 8.83},
        {"Bizerte", 37.27, 9.87},
        {"Gafsa", 34.43, 8.78},
        {"Médenine", 33.35, 10.50},
        {"Tataouine", 32.93, 10.45},
        {"Nabeul", 36.45, 10.74},
        {"Monastir", 35.78, 10.83},
        {"Mahdia", 35.50, 11.06},
        {"Béja", 36.73, 9.18},
        {"Jendouba", 36.50, 8.78},
        {"Le Kef", 36.17, 8.71},
        {"Siliana", 36.08, 9.37},
        {"Zaghouan", 36.40, 10.14},
        {"Sidi Bouzid", 35.04, 9.49},
        {"Kébili", 33.70, 8.97}
};

/* Algérie */

static const double DZ_OUT[][2] =
    {
        {35.1, -2.2},
        {36.5, 1.0},
        {36.8, 3.0},
        {36.9, 5.5},
        {37.1, 8.6},
        {34.6, 8.3},
        {33.2, 8.2},
        {30.2, 9.5},
        {27.2, 9.9},
        {25.0, 11.9},
        {23.5, 11.9},
        {19.1, 12.2},
        {19.1, 4.3},
        {21.6, 1.0},
        {21.0, -3.0},
        {24.9, -4.8},
        {27.3, -8.7},
        {29.5, -8.7},
        {32.0, -2.9}
};

static const CityRow DZ_CITIES[] =
    {
        {"Alger", 36.75, 3.06},
        {"Oran", 35.70, -0.63},
        {"Constantine", 36.37, 6.61},
        {"Annaba", 36.90, 7.77},
        {"Sétif", 36.19, 5.41},
        {"Batna", 35.56, 6.17},
        {"Biskra", 34.85, 5.73},
        {"Ghardaïa", 32.49, 3.67},
        {"Tamanrasset", 22.79, 5.52},
        {"Béchar", 31.62, -2.22},
        {"Tlemcen", 34.88, -1.32}
};

/* Maroc */

static const double MA_OUT[][2] =
    {
        {35.8, -5.9},
        {35.2, -2.9},
        {34.7, -1.9},
        {32.1, -1.2},
        {31.0, -3.5},
        {30.0, -5.5},
        {28.5, -8.7},
        {27.7, -8.7},
        {27.7, -12.9},
        {29.0, -10.0},
        {30.4, -9.8},
        {32.0, -9.3},
        {33.6, -7.6},
        {35.0, -6.5}
};

static const CityRow MA_CITIES[] =
    {
        {"Rabat", 34.02, -6.84},
        {"Casablanca", 33.57, -7.59},
        {"Marrakech", 31.63, -8.01},
        {"Fès", 34.03, -5.00},
        {"Tanger", 35.76, -5.80},
        {"Agadir", 30.43, -9.60},
        {"Oujda", 34.68, -1.91},
        {"Meknès", 33.89, -5.55},
        {"Ouarzazate", 30.92, -6.89}
};

/* Libye */

static const double LY_OUT[][2] =
    {
        {33.1, 11.5},
        {32.9, 13.0},
        {32.5, 15.2},
        {30.3, 18.5},
        {32.6, 20.0},
        {32.9, 22.0},
        {31.6, 25.1},
        {22.0, 25.0},
        {20.0, 24.0},
        {21.9, 15.7},
        {23.5, 14.0},
        {23.5, 11.9},
        {25.0, 11.9},
        {27.2, 9.9},
        {30.2, 9.5},
        {32.2, 10.4}
};

static const CityRow LY_CITIES[] =
    {
        {"Tripoli", 32.89, 13.19},
        {"Benghazi", 32.12, 20.07},
        {"Misrata", 32.38, 15.09},
        {"Sebha", 27.04, 14.43},
        {"Tobrouk", 32.08, 23.98},
        {"Syrte", 31.21, 16.59},
        {"Ghadamès", 30.13, 9.50},
        {"Al Bayda", 32.77, 21.75}
};

/* Égypte */

static const double EG_OUT[][2] =
    {
        {31.6, 25.1},
        {31.3, 29.0},
        {31.5, 31.0},
        {31.2, 32.3},
        {31.2, 34.2},
        {29.5, 34.9},
        {27.8, 34.3},
        {25.5, 34.8},
        {24.1, 35.6},
        {22.0, 36.9},
        {22.0, 25.0}
};

static const CityRow EG_CITIES[] =
    {
        {"Le Caire", 30.04, 31.24},
        {"Alexandrie", 31.20, 29.92},
        {"Louxor", 25.69, 32.64},
        {"Assouan", 24.09, 32.90},
        {"Port-Saïd", 31.27, 32.30},
        {"Suez", 29.97, 32.55},
        {"Hurghada", 27.26, 33.81},
        {"Charm el-Cheikh", 27.91, 34.33},
        {"Marsa Matruh", 31.35, 27.24},
        {"Assiout", 27.18, 31.18}
};

/* France */

static const double FR_OUT[][2] =
    {
        {51.0, 2.5},
        {50.1, 1.6},
        {49.4, 0.2},
        {49.7, -1.6},
        {48.7, -1.6},
        {48.7, -4.5},
        {47.9, -4.7},
        {47.3, -2.5},
        {46.2, -1.2},
        {44.6, -1.2},
        {43.4, -1.8},
        {42.7, 0.0},
        {42.4, 3.2},
        {43.5, 4.0},
        {43.3, 5.4},
        {43.1, 6.5},
        {43.8, 7.5},
        {45.1, 6.9},
        {46.0, 7.0},
        {46.4, 6.1},
        {47.6, 7.5},
        {48.9, 8.2},
        {49.5, 6.4},
        {49.5, 5.5},
        {50.1, 4.8},
        {50.7, 3.2}
};

static const CityRow FR_CITIES[] =
    {
        {"Paris", 48.86, 2.35},
        {"Marseille", 43.30, 5.37},
        {"Lyon", 45.76, 4.84},
        {"Toulouse", 43.60, 1.44},
        {"Nice", 43.70, 7.27},
        {"Nantes", 47.22, -1.55},
        {"Strasbourg", 48.57, 7.75},
        {"Bordeaux", 44.84, -0.58},
        {"Lille", 50.63, 3.06},
        {"Rennes", 48.11, -1.68}
};

/* ------------------------------------------------------------------ */

static void fillGeo(
    EquipmentCountryGeo &g,
    const char *nom,
    const double (*out)[2],
    int nOut,
    const CityRow *c,
    int nC)
{
    g = EquipmentCountryGeo();

    g.nom =
        QString::fromUtf8(nom);

    for (int i = 0; i < nOut; ++i)
    {
        g.contour.append(
            QPointF(
                out[i][0],
                out[i][1]
                )
            );
    }

    for (int i = 0; i < nC; ++i)
    {
        g.addVille(
            QString::fromUtf8(c[i].n),
            c[i].lat,
            c[i].lon
            );
    }
}

/* ------------------------------------------------------------------ */

static bool buildGeo(
    QLocale::Territory t,
    EquipmentCountryGeo &g)
{
    switch (t)
    {
    case QLocale::Tunisia:
        fillGeo(
            g,
            "Tunisie",
            TN_OUT,
            COUNT(TN_OUT),
            TN_CITIES,
            COUNT(TN_CITIES)
            );
        return true;

    case QLocale::Algeria:
        fillGeo(
            g,
            "Algérie",
            DZ_OUT,
            COUNT(DZ_OUT),
            DZ_CITIES,
            COUNT(DZ_CITIES)
            );
        return true;

    case QLocale::Morocco:
        fillGeo(
            g,
            "Maroc",
            MA_OUT,
            COUNT(MA_OUT),
            MA_CITIES,
            COUNT(MA_CITIES)
            );
        return true;

    case QLocale::Libya:
        fillGeo(
            g,
            "Libye",
            LY_OUT,
            COUNT(LY_OUT),
            LY_CITIES,
            COUNT(LY_CITIES)
            );
        return true;

    case QLocale::Egypt:
        fillGeo(
            g,
            "Égypte",
            EG_OUT,
            COUNT(EG_OUT),
            EG_CITIES,
            COUNT(EG_CITIES)
            );
        return true;

    case QLocale::France:
        fillGeo(
            g,
            "France",
            FR_OUT,
            COUNT(FR_OUT),
            FR_CITIES,
            COUNT(FR_CITIES)
            );
        return true;

    default:
        return false;
    }
}

/* ------------------------------------------------------------------ */
/* Utilitaires                                                         */
/* ------------------------------------------------------------------ */

static QFrame *makeCard()
{
    QFrame *f = new QFrame;
    f->setObjectName("card");
    return f;
}

static QLabel *makeCardTitle(
    const QString &t)
{
    QLabel *l = new QLabel(t);
    l->setObjectName("cardTitle");
    return l;
}

/* ------------------------------------------------------------------ */
/* Tri                                                                  */
/* ------------------------------------------------------------------ */

static bool lessNom(
    const Equipement &a,
    const Equipement &b)
{
    return a.nom.localeAwareCompare(b.nom) < 0;
}

static bool lessLocalisation(
    const Equipement &a,
    const Equipement &b)
{
    return a.localisation.localeAwareCompare(
               b.localisation
               ) < 0;
}

static bool lessStatut(
    const Equipement &a,
    const Equipement &b)
{
    return a.statut.localeAwareCompare(
               b.statut
               ) < 0;
}

static bool lessType(
    const Equipement &a,
    const Equipement &b)
{
    return a.type.localeAwareCompare(
               b.type
               ) < 0;
}

/* ------------------------------------------------------------------ */
/* Ajouter un champ dans la grille                                    */
/* ------------------------------------------------------------------ */

static void addField(
    QGridLayout *g,
    int row,
    int col,
    const QString &label,
    QWidget *w)
{
    QLabel *l = new QLabel(label);

    l->setMinimumWidth(115);

    g->addWidget(
        l,
        row,
        col * 2
        );

    g->addWidget(
        w,
        row,
        col * 2 + 1
        );
}

/* ------------------------------------------------------------------ */
/* Constructeur                                                         */
/* ------------------------------------------------------------------ */

WeatherWiseTest::WeatherWiseTest(
    QWidget *parent)
    : QMainWindow(parent),
    m_sortMode(-1),
    m_table(0),
    m_search(0),
    m_sort(0),
    m_donut(0),
    m_actifsLabel(0),
    m_inactifsLabel(0),
    m_maintenanceLabel(0),
    m_map(0),
    m_dateLabel(0),
    m_timeLabel(0),
    m_id(0),
    m_nom(0),
    m_loc(0),
    m_date(0),
    m_statut(0),
    m_type(0)
{
    setWindowTitle(
        "Smart Weather Management - Gestion des équipements"
        );

    setStyleSheet(
        styleSheet()
        );

    detectCountry();

    loadSampleData();

    QWidget *central =
        new QWidget;

    QHBoxLayout *root =
        new QHBoxLayout(central);

    root->setContentsMargins(
        0,
        0,
        0,
        0
        );

    root->setSpacing(0);

    root->addWidget(
        buildSidebar()
        );

    QWidget *right =
        new QWidget;

    QVBoxLayout *rv =
        new QVBoxLayout(right);

    rv->setContentsMargins(
        0,
        0,
        0,
        0
        );

    rv->setSpacing(0);

    rv->addWidget(
        buildHeader()
        );

    QWidget *content =
        new QWidget;

    content->setObjectName(
        "content"
        );

    QGridLayout *g =
        new QGridLayout(content);

    g->setContentsMargins(
        20,
        12,
        20,
        12
        );

    g->setSpacing(12);

    /* Même organisation que la version bureaux */

    g->addWidget(
        buildFormCard(),
        0,
        0,
        1,
        2
        );

    g->addWidget(
        buildToolbar(),
        1,
        0,
        1,
        2
        );

    g->addWidget(
        buildTableCard(),
        2,
        0
        );

    QWidget *col =
        new QWidget;

    QVBoxLayout *cv =
        new QVBoxLayout(col);

    cv->setContentsMargins(
        0,
        0,
        0,
        0
        );

    cv->setSpacing(12);

    cv->addWidget(
        buildStatsCard(),
        1
        );

    cv->addWidget(
        buildMapCard(),
        1
        );

    g->addWidget(
        col,
        2,
        1
        );

    g->setColumnStretch(
        0,
        3
        );

    g->setColumnStretch(
        1,
        2
        );

    g->setRowStretch(
        2,
        1
        );

    rv->addWidget(
        content,
        1
        );

    root->addWidget(
        right,
        1
        );

    setCentralWidget(
        central
        );

    onClear();

    refreshTable();

    QTimer *clock =
        new QTimer(this);

    connect(
        clock,
        SIGNAL(timeout()),
        this,
        SLOT(updateClock())
        );

    clock->start(1000);

    updateClock();
}

/* ------------------------------------------------------------------ */
/* Pays                                                                 */
/* ------------------------------------------------------------------ */

void WeatherWiseTest::detectCountry()
{
    QLocale::Territory cand[3];

    cand[0] =
        kForcedCountry;

    cand[1] =
        QLocale::system().territory();

    cand[2] =
        QTimeZone::systemTimeZone().territory();

    for (int i = 0; i < 3; ++i)
    {
        if (
            cand[i] != QLocale::AnyTerritory &&
            buildGeo(
                cand[i],
                m_geo
                )
            )
        {
            return;
        }
    }

    buildGeo(
        QLocale::Tunisia,
        m_geo
        );

    QLocale::Territory detected =
        cand[1] != QLocale::AnyTerritory
            ? cand[1]
            : cand[2];

    m_geoNote =
        QString::fromUtf8(
            " (carte de %1 indisponible)"
            ).arg(
                QLocale::territoryToString(
                    detected
                    )
                );
}

/* ------------------------------------------------------------------ */
/* Données exemples                                                     */
/* ------------------------------------------------------------------ */

void WeatherWiseTest::loadSampleData()
{
    m_equipements.clear();

    struct Row
    {
        const char *nom;
        int ville;
        const char *statut;
        const char *type;
        int y;
        int m;
        int d;
    };

    static const Row rows[] =
        {
            {
                "Station météo principale",
                0,
                "Actif",
                "Station météo",
                2018,
                3,
                12
            },

            {
                "Thermomètre central",
                1,
                "Actif",
                "Thermomètre",
                2019,
                7,
                25
            },

            {
                "Baromètre Sousse",
                2,
                "Actif",
                "Baromètre",
                2020,
                11,
                10
            },

            {
                "Anémomètre Kairouan",
                3,
                "Inactif",
                "Anémomètre",
                2021,
                5,
                3
            },

            {
                "Pluviomètre Gabès",
                4,
                "Actif",
                "Pluviomètre",
                2022,
                9,
                17
            },

            {
                "Hygromètre Tozeur",
                5,
                "Actif",
                "Hygromètre",
                2023,
                2,
                28
            },

            {
                "Capteur Kasserine",
                6,
                "Inactif",
                "Capteur météo",
                2023,
                6,
                14
            },

            {
                "Station Bizerte",
                7,
                "Actif",
                "Station météo",
                2023,
                12,
                22
            },

            {
                "Capteur Gafsa",
                8,
                "En maintenance",
                "Capteur météo",
                2024,
                4,
                8
            },

            {
                "Station Médenine",
                9,
                "Actif",
                "Station météo",
                2024,
                10,
                16
            }
        };

    const int count =
        sizeof(rows) / sizeof(rows[0]);

    for (int i = 0; i < count; ++i)
    {
        Equipement e;

        e.id =
            QString("E%1")
                .arg(
                    i + 1,
                    3,
                    10,
                    QChar('0')
                    );

        e.nom =
            QString::fromUtf8(
                rows[i].nom
                );

        if (
            !m_geo.villes.isEmpty() &&
            rows[i].ville < m_geo.villes.size()
            )
        {
            e.localisation =
                m_geo.villes.at(
                    rows[i].ville
                    );

        }
        else
        {
            e.localisation =
                "Tunis";

        }

        e.statut =
            QString::fromUtf8(
                rows[i].statut
                );

        e.type =
            QString::fromUtf8(
                rows[i].type
                );

        e.dateInstallation =
            QDate(
                rows[i].y,
                rows[i].m,
                rows[i].d
                );

        m_equipements.append(e);
    }
}

/* ------------------------------------------------------------------ */

QString WeatherWiseTest::nextId() const
{
    int mx = 0;

    for (int i = 0;
         i < m_equipements.size();
         ++i)
    {
        QString id =
            m_equipements.at(i).id;

        if (id.length() > 1)
        {
            mx =
                qMax(
                    mx,
                    id.mid(1).toInt()
                    );
        }
    }

    return QString("E%1")
        .arg(
            mx + 1,
            3,
            10,
            QChar('0')
            );
}

/* ------------------------------------------------------------------ */
/* Sidebar                                                              */
/* ------------------------------------------------------------------ */

QWidget *WeatherWiseTest::buildSidebar()
{
    QFrame *side =
        new QFrame;

    side->setObjectName(
        "sidebar"
        );

    side->setFixedWidth(
        300
        );

    QVBoxLayout *v =
        new QVBoxLayout(side);

    v->setContentsMargins(
        0,
        14,
        0,
        14
        );

    v->setSpacing(6);

    QLabel *sun =
        new QLabel(
            QString::fromUtf8("☀")
            );

    sun->setObjectName(
        "sunIcon"
        );

    sun->setAlignment(
        Qt::AlignCenter
        );

    v->addWidget(sun);

    QLabel *brand =
        new QLabel(
            "SMART WEATHER\nMANAGEMENT"
            );

    brand->setObjectName(
        "brand"
        );

    brand->setAlignment(
        Qt::AlignCenter
        );

    v->addWidget(brand);

    v->addSpacing(26);

    struct Item
    {
        const char *text;
        bool active;
    };

    static const Item items[] =
        {
            {
                "⌂   Accueil",
                false
            },

            {
                "🏢   Gestion de bureau",
                false
            },

            {
                "⚙   Gestion des équipements",
                true
            },

            {
                "👷   Gestion des techniciens",
                false
            },

            {
                "📡   Gestion des stations",
                false
            },

            {
                "👥   Gestion des utilisateurs",
                false
            }
        };

    for (int i = 0; i < 6; ++i)
    {
        QPushButton *b =
            new QPushButton(
                QString::fromUtf8(
                    items[i].text
                    )
                );

        b->setObjectName(
            "nav"
            );

        b->setProperty(
            "active",
            items[i].active
            );

        b->setCursor(
            Qt::PointingHandCursor
            );

        v->addWidget(b);
    }

    v->addStretch();

    return side;
}

/* ------------------------------------------------------------------ */
/* Header                                                               */
/* ------------------------------------------------------------------ */

QWidget *WeatherWiseTest::buildHeader()
{
    QFrame *h =
        new QFrame;

    h->setObjectName(
        "header"
        );

    h->setFixedHeight(
        72
        );

    QHBoxLayout *l =
        new QHBoxLayout(h);

    l->setContentsMargins(
        34,
        6,
        34,
        6
        );

    QVBoxLayout *tv =
        new QVBoxLayout;

    tv->setSpacing(0);

    QLabel *t =
        new QLabel(
            "Gestion des équipements"
            );

    t->setObjectName(
        "headerTitle"
        );

    QLabel *s =
        new QLabel(
            QString::fromUtf8(
                "Gérez les équipements météorologiques et leurs informations"
                )
            );

    s->setObjectName(
        "headerSub"
        );

    tv->addStretch();
    tv->addWidget(t);
    tv->addWidget(s);
    tv->addStretch();

    l->addLayout(tv);

    l->addStretch();

    m_dateLabel =
        new QLabel;

    m_dateLabel->setObjectName(
        "headerInfo"
        );

    m_timeLabel =
        new QLabel;

    m_timeLabel->setObjectName(
        "headerInfo"
        );

    QLabel *dot =
        new QLabel(
            "•"
            );

    dot->setObjectName(
        "headerInfo"
        );

    QLabel *admin =
        new QLabel(
            QString::fromUtf8(
                "Administrateur  ▼"
                )
            );

    admin->setObjectName(
        "headerInfo"
        );

    l->addWidget(
        m_dateLabel
        );

    l->addSpacing(22);

    l->addWidget(
        m_timeLabel
        );

    l->addSpacing(22);

    l->addWidget(
        dot
        );

    l->addSpacing(8);

    l->addWidget(
        admin
        );

    return h;
}

/* ------------------------------------------------------------------ */

void WeatherWiseTest::updateClock()
{
    const QDateTime now =
        QDateTime::currentDateTime();

    m_dateLabel->setText(
        now.toString(
            "dd/MM/yyyy"
            )
        );

    m_timeLabel->setText(
        now.toString(
            "HH:mm"
            )
        );
}

/* ------------------------------------------------------------------ */
/* Formulaire                                                           */
/* ------------------------------------------------------------------ */

QWidget *WeatherWiseTest::buildFormCard()
{
    QFrame *card =
        makeCard();

    QVBoxLayout *v =
        new QVBoxLayout(card);

    v->setContentsMargins(
        20,
        10,
        20,
        12
        );

    v->setSpacing(8);

    v->addWidget(
        makeCardTitle(
            QString::fromUtf8(
                "⚙  Informations de l'équipement"
                )
            )
        );

    m_id =
        new QLineEdit;

    m_id->setPlaceholderText(
        QString::fromUtf8(
            "Généré automatiquement si vide (ex. E011)"
            )
        );

    m_nom =
        new QLineEdit;

    m_loc =
        new QComboBox;

    m_loc->addItems(
        m_geo.villes
        );

    m_date =
        new QDateEdit(
            QDate::currentDate()
            );

    m_date->setCalendarPopup(
        true
        );

    m_date->setDisplayFormat(
        "dd/MM/yyyy"
        );

    m_statut =
        new QComboBox;

    m_statut->addItem(
        "Actif"
        );

    m_statut->addItem(
        "Inactif"
        );

    m_statut->addItem(
        "En maintenance"
        );

    m_type =
        new QComboBox;

    m_type->addItem(
        "Station météo"
        );

    m_type->addItem(
        "Thermomètre"
        );

    m_type->addItem(
        "Baromètre"
        );

    m_type->addItem(
        "Anémomètre"
        );

    m_type->addItem(
        "Pluviomètre"
        );

    m_type->addItem(
        "Hygromètre"
        );

    m_type->addItem(
        "Capteur météo"
        );

    QGridLayout *g =
        new QGridLayout;

    g->setHorizontalSpacing(
        14
        );

    g->setVerticalSpacing(
        8
        );

    g->setColumnStretch(
        1,
        1
        );

    g->setColumnStretch(
        3,
        1
        );

    /*
     * Même grille générale que l'ancien formulaire.
     */

    addField(
        g,
        0,
        0,
        "ID Équipement :",
        m_id
        );

    addField(
        g,
        0,
        1,
        "Nom :",
        m_nom
        );

    addField(
        g,
        1,
        0,
        "Localisation :",
        m_loc
        );

    addField(
        g,
        2,
        0,
        "Date d'installation :",
        m_date
        );

    addField(
        g,
        2,
        1,
        "Statut :",
        m_statut
        );

    addField(
        g,
        3,
        0,
        "Type d'équipement :",
        m_type
        );

    v->addLayout(g);

    return card;
}

/* ------------------------------------------------------------------ */
/* Toolbar                                                              */
/* ------------------------------------------------------------------ */

QWidget *WeatherWiseTest::buildToolbar()
{
    QFrame *card =
        makeCard();

    QHBoxLayout *l =
        new QHBoxLayout(card);

    l->setContentsMargins(
        14,
        8,
        14,
        8
        );

    l->setSpacing(8);

    QPushButton *bAdd =
        new QPushButton(
            QString::fromUtf8(
                "＋  Ajouter"
                )
            );

    bAdd->setObjectName(
        "primary"
        );

    QPushButton *bEdit =
        new QPushButton(
            QString::fromUtf8(
                "✎  Modifier"
                )
            );

    QPushButton *bDel =
        new QPushButton(
            QString::fromUtf8(
                "▣  Supprimer"
                )
            );

    bDel->setObjectName(
        "danger"
        );

    QPushButton *bClear =
        new QPushButton(
            QString::fromUtf8(
                "↻  Vider"
                )
            );

    l->addWidget(bAdd);
    l->addWidget(bEdit);
    l->addWidget(bDel);
    l->addWidget(bClear);

    l->addSpacing(10);

    l->addWidget(
        new QLabel(
            QString::fromUtf8(
                "↕  Trier par"
                )
            )
        );

    m_sort =
        new QComboBox;

    m_sort->addItem(
        "Nom"
        );

    m_sort->addItem(
        "Localisation"
        );

    m_sort->addItem(
        "Statut"
        );

    m_sort->addItem(
        "Type"
        );

    m_sort->setMinimumWidth(
        180
        );

    l->addWidget(
        m_sort
        );

    QPushButton *bSort =
        new QPushButton(
            "Trier"
            );

    l->addWidget(
        bSort
        );

    m_search =
        new QLineEdit;

    m_search->setPlaceholderText(
        QString::fromUtf8(
            "Rechercher par ID, nom, localisation, statut, date ou type..."
            )
        );

    l->addWidget(
        m_search,
        1
        );

    QPushButton *bFind =
        new QPushButton(
            QString::fromUtf8(
                "🔍"
                )
            );

    l->addWidget(
        bFind
        );

    QPushButton *bPdf =
        new QPushButton(
            QString::fromUtf8(
                "▤  Exporter en PDF"
                )
            );

    l->addWidget(
        bPdf
        );

    connect(
        bAdd,
        SIGNAL(clicked()),
        this,
        SLOT(onAdd())
        );

    connect(
        bEdit,
        SIGNAL(clicked()),
        this,
        SLOT(onEdit())
        );

    connect(
        bDel,
        SIGNAL(clicked()),
        this,
        SLOT(onDelete())
        );

    connect(
        bClear,
        SIGNAL(clicked()),
        this,
        SLOT(onClear())
        );

    connect(
        bSort,
        SIGNAL(clicked()),
        this,
        SLOT(onSort())
        );

    connect(
        m_sort,
        SIGNAL(currentIndexChanged(int)),
        this,
        SLOT(onSort())
        );

    connect(
        bFind,
        SIGNAL(clicked()),
        this,
        SLOT(onSearch())
        );

    connect(
        m_search,
        SIGNAL(returnPressed()),
        this,
        SLOT(onSearch())
        );

    connect(
        m_search,
        SIGNAL(textChanged(QString)),
        this,
        SLOT(onSearch())
        );

    connect(
        bPdf,
        SIGNAL(clicked()),
        this,
        SLOT(exportPdf())
        );

    return card;
}

/* ------------------------------------------------------------------ */
/* Tableau                                                              */
/* ------------------------------------------------------------------ */

QWidget *WeatherWiseTest::buildTableCard()
{
    QFrame *card =
        makeCard();

    QVBoxLayout *v =
        new QVBoxLayout(card);

    v->setContentsMargins(
        16,
        14,
        16,
        14
        );

    v->setSpacing(8);

    QHBoxLayout *top =
        new QHBoxLayout;

    top->addWidget(
        makeCardTitle(
            QString::fromUtf8(
                "▤  Liste des équipements"
                )
            )
        );

    top->addStretch();

    QLabel *hint =
        new QLabel(
            QString::fromUtf8(
                "Sélectionnez une ligne pour modifier les informations"
                )
            );

    hint->setObjectName(
        "hint"
        );

    top->addWidget(
        hint
        );

    v->addLayout(
        top
        );

    m_table =
        new QTableWidget(
            0,
            7
            );

    QStringList headers;

    headers
        << "ID Équipement"
        << "Nom"
        << "Localisation"
        << "Date d'installation"
        << "Statut"
        << "Type d'équipement";

    m_table->setHorizontalHeaderLabels(
        headers
        );

    m_table->verticalHeader()
        ->setDefaultSectionSize(
            34
            );

    m_table->horizontalHeader()
        ->setSectionResizeMode(
            QHeaderView::Stretch
            );

    m_table->setSelectionBehavior(
        QAbstractItemView::SelectRows
        );

    m_table->setSelectionMode(
        QAbstractItemView::SingleSelection
        );

    m_table->setEditTriggers(
        QAbstractItemView::NoEditTriggers
        );

    m_table->setFocusPolicy(
        Qt::NoFocus
        );

    m_table->setMinimumHeight(
        120
        );

    v->addWidget(
        m_table,
        1
        );

    connect(
        m_table,
        SIGNAL(itemSelectionChanged()),
        this,
        SLOT(onRowSelected())
        );

    return card;
}

/* ------------------------------------------------------------------ */
/* Refresh table                                                        */
/* ------------------------------------------------------------------ */

void WeatherWiseTest::refreshTable()
{
    const QString q =
        m_search->text().trimmed();

    QVector<Equipement> list;

    for (int i = 0;
         i < m_equipements.size();
         ++i)
    {
        const Equipement &e =
            m_equipements.at(i);

        const QString date =
            e.dateInstallation.toString(
                "dd/MM/yyyy"
                );

        bool match =
            q.isEmpty()
            || e.id.contains(
                q,
                Qt::CaseInsensitive
                )
            || e.nom.contains(
                q,
                Qt::CaseInsensitive
                )
            || e.localisation.contains(
                q,
                Qt::CaseInsensitive
                )
            || e.statut.contains(
                q,
                Qt::CaseInsensitive
                )
            || date.contains(
                q,
                Qt::CaseInsensitive
                )
            || e.type.contains(
                q,
                Qt::CaseInsensitive
                );

        if (match)
            list.append(e);
    }

    if (m_sortMode == 0)
    {
        std::sort(
            list.begin(),
            list.end(),
            lessNom
            );
    }
    else if (m_sortMode == 1)
    {
        std::sort(
            list.begin(),
            list.end(),
            lessLocalisation
            );
    }
    else if (m_sortMode == 2)
    {
        std::sort(
            list.begin(),
            list.end(),
            lessStatut
            );
    }
    else if (m_sortMode == 3)
    {
        std::sort(
            list.begin(),
            list.end(),
            lessType
            );
    }

    m_table->setRowCount(0);

    m_table->setRowCount(
        list.size()
        );

    for (int r = 0;
         r < list.size();
         ++r)
    {
        const Equipement &e =
            list.at(r);

        m_table->setItem(
            r,
            0,
            new QTableWidgetItem(
                e.id
                )
            );

        m_table->setItem(
            r,
            1,
            new QTableWidgetItem(
                e.nom
                )
            );

        m_table->setItem(
            r,
            2,
            new QTableWidgetItem(
                e.localisation
                )
            );

        m_table->setItem(
            r,
            3,
            new QTableWidgetItem(
                e.dateInstallation.toString(
                    "dd/MM/yyyy"
                    )
                )
            );

        QTableWidgetItem *st =
            new QTableWidgetItem(
                e.statut
                );

        if (e.statut == "Actif")
        {
            st->setForeground(
                QBrush(
                    QColor("#16a068")
                    )
                );
        }
        else if (e.statut == "Inactif")
        {
            st->setForeground(
                QBrush(
                    QColor("#e5474f")
                    )
                );
        }
        else
        {
            st->setForeground(
                QBrush(
                    QColor("#f0a000")
                    )
                );
        }

        m_table->setItem(
            r,
            4,
            st
            );

        m_table->setItem(
            r,
            5,
            new QTableWidgetItem(
                e.type
                )
            );
    }

    updateStats();

    m_map->setEquipements(
        m_equipements
        );
}

/* ------------------------------------------------------------------ */
/* Statistiques                                                         */
/* ------------------------------------------------------------------ */

QWidget *WeatherWiseTest::buildStatsCard()
{
    QFrame *card =
        makeCard();

    QVBoxLayout *v =
        new QVBoxLayout(card);

    v->setContentsMargins(
        16,
        10,
        16,
        10
        );

    v->setSpacing(6);

    v->addWidget(
        makeCardTitle(
            QString::fromUtf8(
                "📊  Statistiques des équipements"
                )
            )
        );

    m_donut =
        new EquipmentDonutChart;

    QLabel *dotA =
        new QLabel(
            QString::fromUtf8("●")
            );

    dotA->setStyleSheet(
        "color:#16a068; background:transparent;"
        );

    QLabel *dotI =
        new QLabel(
            QString::fromUtf8("●")
            );

    dotI->setStyleSheet(
        "color:#e5474f; background:transparent;"
        );

    QLabel *dotM =
        new QLabel(
            QString::fromUtf8("●")
            );

    dotM->setStyleSheet(
        "color:#f0a000; background:transparent;"
        );

    m_actifsLabel =
        new QLabel;

    m_inactifsLabel =
        new QLabel;

    m_maintenanceLabel =
        new QLabel;

    QGridLayout *lg =
        new QGridLayout;

    lg->setHorizontalSpacing(
        10
        );

    lg->setVerticalSpacing(
        8
        );

    lg->addWidget(
        dotA,
        0,
        0
        );

    lg->addWidget(
        new QLabel("Actifs"),
        0,
        1
        );

    lg->addWidget(
        m_actifsLabel,
        0,
        2
        );

    lg->addWidget(
        dotI,
        1,
        0
        );

    lg->addWidget(
        new QLabel("Inactifs"),
        1,
        1
        );

    lg->addWidget(
        m_inactifsLabel,
        1,
        2
        );

    lg->addWidget(
        dotM,
        2,
        0
        );

    lg->addWidget(
        new QLabel("Maintenance"),
        2,
        1
        );

    lg->addWidget(
        m_maintenanceLabel,
        2,
        2
        );

    QHBoxLayout *body =
        new QHBoxLayout;

    body->addWidget(
        m_donut,
        1
        );

    body->addLayout(
        lg
        );

    v->addLayout(
        body,
        1
        );

    return card;
}

/* ------------------------------------------------------------------ */

void WeatherWiseTest::updateStats()
{
    int actifs = 0;
    int inactifs = 0;
    int maintenance = 0;

    for (int k = 0;
         k < m_equipements.size();
         ++k)
    {
        const Equipement &e =
            m_equipements.at(k);

        if (e.statut == "Actif")
            ++actifs;
        else if (e.statut == "Inactif")
            ++inactifs;
        else
            ++maintenance;
    }

    const int total =
        actifs +
        inactifs +
        maintenance;

    /*
     * Le donut conserve le même fonctionnement
     * visuel que l'ancienne version :
     * actifs / non-actifs.
     */
    m_donut->setValues(
        actifs,
        inactifs + maintenance
        );

    m_actifsLabel->setText(
        QString("%1 (%2%)")
            .arg(actifs)
            .arg(
                total
                    ? qRound(
                          100.0 *
                          actifs /
                          total
                          )
                    : 0
                )
        );

    m_inactifsLabel->setText(
        QString("%1 (%2%)")
            .arg(inactifs)
            .arg(
                total
                    ? qRound(
                          100.0 *
                          inactifs /
                          total
                          )
                    : 0
                )
        );

    m_maintenanceLabel->setText(
        QString("%1 (%2%)")
            .arg(maintenance)
            .arg(
                total
                    ? qRound(
                          100.0 *
                          maintenance /
                          total
                          )
                    : 0
                )
        );
}

/* ------------------------------------------------------------------ */
/* Carte                                                                 */
/* ------------------------------------------------------------------ */

QWidget *WeatherWiseTest::buildMapCard()
{
    QFrame *card =
        makeCard();

    QVBoxLayout *v =
        new QVBoxLayout(card);

    v->setContentsMargins(
        14,
        12,
        14,
        14
        );

    QLabel *mt =
        makeCardTitle(
            QString::fromUtf8(
                "📍  Localisation géographique – "
                )
            +
            m_geo.nom
            +
            m_geoNote
            );

    mt->setWordWrap(
        true
        );

    v->addWidget(
        mt
        );

    m_map =
        new EquipmentMapWidget;

    m_map->setCountry(
        m_geo
        );

    v->addWidget(
        m_map,
        1
        );

    return card;
}

/* ------------------------------------------------------------------ */
/* Lecture formulaire                                                   */
/* ------------------------------------------------------------------ */

bool WeatherWiseTest::readForm(
    Equipement &e)
{
    const QString nom =
        m_nom->text().trimmed();

    if (nom.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Champs obligatoires",
            QString::fromUtf8(
                "Le nom de l'équipement est obligatoire."
                )
            );

        return false;
    }

    e.nom =
        nom;

    e.localisation =
        m_loc->currentText();

    e.dateInstallation =
        m_date->date();

    e.statut =
        m_statut->currentText();

    e.type =
        m_type->currentText();

    return true;
}

/* ------------------------------------------------------------------ */
/* Ajouter                                                              */
/* ------------------------------------------------------------------ */

void WeatherWiseTest::onAdd()
{
    Equipement e;

    if (!readForm(e))
        return;

    QString id =
        m_id->text()
            .trimmed()
            .toUpper();

    if (id.isEmpty())
        id = nextId();

    for (int i = 0;
         i < m_equipements.size();
         ++i)
    {
        if (
            m_equipements.at(i).id
            == id
            )
        {
            QMessageBox::warning(
                this,
                "ID existant",
                QString::fromUtf8(
                    "Un équipement avec l'ID %1 existe déjà."
                    ).arg(id)
                );

            return;
        }
    }

    e.id =
        id;

    m_equipements.append(
        e
        );

    onClear();

    refreshTable();
}

/* ------------------------------------------------------------------ */
/* Modifier                                                             */
/* ------------------------------------------------------------------ */

void WeatherWiseTest::onEdit()
{
    if (m_editId.isEmpty())
    {
        QMessageBox::information(
            this,
            "Modifier",
            QString::fromUtf8(
                "Sélectionnez d'abord une ligne dans la liste."
                )
            );

        return;
    }

    Equipement ne;

    if (!readForm(ne))
        return;

    ne.id =
        m_editId;

    for (int i = 0;
         i < m_equipements.size();
         ++i)
    {
        if (
            m_equipements.at(i).id
            == m_editId
            )
        {
            m_equipements[i] =
                ne;

            break;
        }
    }

    onClear();

    refreshTable();
}

/* ------------------------------------------------------------------ */
/* Supprimer                                                            */
/* ------------------------------------------------------------------ */

void WeatherWiseTest::onDelete()
{
    if (m_editId.isEmpty())
    {
        QMessageBox::information(
            this,
            "Supprimer",
            QString::fromUtf8(
                "Sélectionnez d'abord une ligne dans la liste."
                )
            );

        return;
    }

    const QMessageBox::StandardButton rep =
        QMessageBox::question(
            this,
            "Confirmation",
            QString::fromUtf8(
                "Supprimer l'équipement %1 ?"
                ).arg(m_editId)
            );

    if (rep != QMessageBox::Yes)
        return;

    for (int i =
         m_equipements.size() - 1;
         i >= 0;
         --i)
    {
        if (
            m_equipements.at(i).id
            == m_editId
            )
        {
            m_equipements.remove(i);
        }
    }

    onClear();

    refreshTable();
}

/* ------------------------------------------------------------------ */
/* Vider                                                                */
/* ------------------------------------------------------------------ */

void WeatherWiseTest::onClear()
{
    m_editId.clear();

    m_id->clear();

    m_id->setReadOnly(
        false
        );

    m_nom->clear();

    m_loc->setCurrentIndex(
        0
        );

    m_date->setDate(
        QDate::currentDate()
        );

    m_statut->setCurrentIndex(
        0
        );

    m_type->setCurrentIndex(
        0
        );

    m_table->clearSelection();
}

/* ------------------------------------------------------------------ */
/* Tri                                                                  */
/* ------------------------------------------------------------------ */

void WeatherWiseTest::onSort()
{
    m_sortMode =
        m_sort->currentIndex();

    refreshTable();
}

/* ------------------------------------------------------------------ */
/* Recherche                                                            */
/* ------------------------------------------------------------------ */

void WeatherWiseTest::onSearch()
{
    refreshTable();
}

/* ------------------------------------------------------------------ */
/* Sélection ligne                                                      */
/* ------------------------------------------------------------------ */

void WeatherWiseTest::onRowSelected()
{
    const QList<QTableWidgetItem *> sel =
        m_table->selectedItems();

    if (sel.isEmpty())
        return;

    QTableWidgetItem *first =
        m_table->item(
            sel.first()->row(),
            0
            );

    if (!first)
        return;

    const QString id =
        first->text();

    for (int i = 0;
         i < m_equipements.size();
         ++i)
    {
        const Equipement &e =
            m_equipements.at(i);

        if (e.id != id)
            continue;

        m_editId =
            id;

        m_id->setText(
            e.id
            );

        m_id->setReadOnly(
            true
            );

        m_nom->setText(
            e.nom
            );

        m_loc->setCurrentText(
            e.localisation
            );

        m_date->setDate(
            e.dateInstallation
            );

        m_statut->setCurrentText(
            e.statut
            );

        m_type->setCurrentText(
            e.type
            );

        return;
    }
}

/* ------------------------------------------------------------------ */
/* Export PDF                                                           */
/* ------------------------------------------------------------------ */

void WeatherWiseTest::exportPdf()
{
    const QString path =
        QFileDialog::getSaveFileName(
            this,
            "Exporter en PDF",
            "equipements.pdf",
            "PDF (*.pdf)"
            );

    if (path.isEmpty())
        return;

    QString html =
        QString::fromUtf8(
            "<h2 style='color:#0f3d91'>"
            "Liste des équipements"
            "</h2>"

            "<table border='1' "
            "cellspacing='0' "
            "cellpadding='6' "
            "width='100%'>"

            "<tr style='background:#e8f0fc'>"

            "<th>ID</th>"
            "<th>Nom</th>"
            "<th>Localisation</th>"
            "<th>Date d'installation</th>"
            "<th>Statut</th>"
            "<th>Type</th>"

            "</tr>"
            );

    for (int i = 0;
         i < m_equipements.size();
         ++i)
    {
        const Equipement &e =
            m_equipements.at(i);

        html +=
            QString(
                "<tr>"
                "<td>%1</td>"
                "<td>%2</td>"
                "<td>%3</td>"
                "<td>%4</td>"
                "<td>%5</td>"
                "<td>%6</td>"
                "</tr>"
                )
                .arg(
                    e.id
                    )
                .arg(
                    e.nom
                    )
                .arg(
                    e.localisation
                    )
                .arg(
                    e.dateInstallation.toString(
                        "dd/MM/yyyy"
                        )
                    )
                .arg(
                    e.statut
                    )
                .arg(
                    e.type
                    );
    }

    html +=
        "</table>";

    QPdfWriter writer(
        path
        );

    writer.setPageSize(
        QPageSize(
            QPageSize::A4
            )
        );

    writer.setPageOrientation(
        QPageLayout::Landscape
        );

    QTextDocument doc;

    doc.setHtml(
        html
        );

    doc.print(
        &writer
        );

    QMessageBox::information(
        this,
        "Export",
        QString::fromUtf8(
            "PDF exporté avec succès :\n"
            )
            +
            path
        );
}

/* ------------------------------------------------------------------ */
/* Style                                                                */
/* ------------------------------------------------------------------ */

QString WeatherWiseTest::styleSheet()
{
    return QString::fromUtf8(
        R"QSS(

QMainWindow, #content {
    background: #f4f7fb;
}

#sidebar {
    background: #17407e;
}

#sunIcon {
    color: #ffc61a;
    font-size: 46px;
    background: transparent;
}

#brand {
    color: white;
    font-size: 17px;
    font-weight: 700;
    background: transparent;
}

QPushButton#nav {
    color: white;
    text-align: left;
    padding: 13px 22px;
    border: none;
    border-radius: 8px;
    font-size: 14px;
    background: transparent;
    margin: 0px 6px;
}

QPushButton#nav:hover {
    background: rgba(255,255,255,0.08);
}

QPushButton#nav[active="true"] {
    background: #2f78d1;
    font-weight: 700;
}

#header {
    background: #2e7ccc;
}

#headerTitle {
    color: white;
    font-size: 26px;
    font-weight: 700;
    background: transparent;
}

#headerSub {
    color: #e3eefc;
    font-size: 13px;
    background: transparent;
}

#headerInfo {
    color: white;
    font-size: 14px;
    font-weight: 700;
    background: transparent;
}

#card {
    background: white;
    border: 1px solid #e3e8f0;
    border-radius: 10px;
}

#card QLabel {
    background: transparent;
    color: #24344f;
}

#card #cardTitle {
    font-size: 16px;
    font-weight: 700;
    color: #0f2a52;
}

#card #hint {
    font-size: 12px;
    color: #7b8aa3;
}

QPushButton {
    background: white;
    border: 1px solid #cfd8e6;
    border-radius: 6px;
    padding: 8px 16px;
    color: #24344f;
    font-size: 14px;
}

QPushButton:hover {
    background: #f1f5fb;
}

QPushButton#primary {
    background: #1d63d8;
    color: white;
    border: none;
    font-weight: 600;
}

QPushButton#primary:hover {
    background: #1852b8;
}

QPushButton#danger {
    color: #d93a44;
    font-weight: 600;
}

QLineEdit,
QComboBox,
QDateEdit {
    background: white;
    border: 1px solid #cfd8e6;
    border-radius: 6px;
    padding: 4px 10px;
    min-height: 26px;
    color: #24344f;
    font-size: 14px;
}

QLineEdit:focus,
QComboBox:focus,
QDateEdit:focus {
    border: 1px solid #1d63d8;
}

QLineEdit:read-only {
    background: #f4f7fb;
}

QTableWidget {
    background: white;
    border: 1px solid #e3e8f0;
    gridline-color: #e8edf5;
    alternate-background-color: #f8fafd;
    selection-background-color: #dbe9fb;
    selection-color: #24344f;
    color: #24344f;
    font-size: 14px;
}

QTableWidget::item {
    padding-left: 6px;
}

QHeaderView::section {
    background: #f1f5fb;
    color: #24344f;
    border: none;
    border-right: 1px solid #e3e8f0;
    border-bottom: 1px solid #e3e8f0;
    padding: 10px 6px;
    font-weight: 700;
}

QTableCornerButton::section {
    background: #f1f5fb;
    border: none;
}

)QSS"
        );
}