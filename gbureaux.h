#ifndef GBUREAUX_H
#define GBUREAUX_H

#include <QMainWindow>
#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QPolygonF>
#include <QPointF>
#include <QVector>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QDate>
#include <QLocale>
#include <QtMath>

class QTableWidget;
class QLineEdit;
class QComboBox;
class QDateEdit;
class QLabel;
class QPushButton;
class QScrollArea;
class QVBoxLayout;

/* ------------------------------------------------------------------ */
/*  Structure de données                                              */
/* ------------------------------------------------------------------ */
struct Bureau {
    QString id;
    QString nom;
    QString prenom;
    QString localisation;
    QDate   dateOuverture;
    bool    actif;

    QString responsable() const { return prenom + " " + nom; }
};

/* ------------------------------------------------------------------ */
/*  Données géographiques d'un pays                                   */
/* ------------------------------------------------------------------ */
struct CountryGeo {
    QString nom;                    // ex. "Tunisie"
    QVector<QPointF> contour;       // contour simplifié (latitude, longitude)
    QStringList villes;             // villes proposées (ordre d'affichage)
    QHash<QString, QPointF> coord;  // ville -> (latitude, longitude)

    void addVille(const QString &n, double lat, double lon)
    {
        villes << n;
        coord.insert(n, QPointF(lat, lon));
    }
};

/* ------------------------------------------------------------------ */
/*  Graphique en anneau (Actifs / Inactifs)                           */
/* ------------------------------------------------------------------ */
class DonutChart : public QWidget
{
public:
    explicit DonutChart(QWidget *parent = 0)
        : QWidget(parent), m_actifs(0), m_inactifs(0)
    {
        setMinimumSize(140, 140);
    }

    void setValues(int actifs, int inactifs)
    {
        m_actifs = actifs;
        m_inactifs = inactifs;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        const int total = m_actifs + m_inactifs;
        const int side = qMin(width(), height()) - 16;
        const qreal penW = side * 0.16;
        QRectF r((width() - side) / 2.0, (height() - side) / 2.0, side, side);
        r.adjust(penW / 2, penW / 2, -penW / 2, -penW / 2);

        QPen pen;
        pen.setWidthF(penW);
        pen.setCapStyle(Qt::FlatCap);

        if (total == 0) {
            pen.setColor(QColor("#e5e7eb"));
            p.setPen(pen);
            p.drawArc(r, 0, 360 * 16);
        } else {
            const int start = 90 * 16;
            const int spanA = -static_cast<int>(360.0 * 16.0 * m_actifs / total);
            const int spanI = -360 * 16 - spanA;

            pen.setColor(QColor("#16a068"));
            p.setPen(pen);
            p.drawArc(r, start, spanA);

            if (m_inactifs > 0) {
                pen.setColor(QColor("#e5474f"));
                p.setPen(pen);
                p.drawArc(r, start + spanA, spanI);
            }
        }

        p.setPen(QColor("#0f2a52"));
        QFont f = font();
        f.setPointSize(22);
        f.setBold(true);
        p.setFont(f);
        p.drawText(QRectF(0, height() / 2.0 - 30, width(), 32), Qt::AlignCenter,
                   QString::number(total));

        f.setPointSize(10);
        f.setBold(false);
        p.setFont(f);
        p.drawText(QRectF(0, height() / 2.0 + 2, width(), 20), Qt::AlignCenter, "bureaux");
    }

private:
    int m_actifs;
    int m_inactifs;
};

/* ------------------------------------------------------------------ */
/*  Carte du pays de l'utilisateur (dessinée avec QPainter)           */
/* ------------------------------------------------------------------ */
class MapWidget : public QWidget
{
public:
    explicit MapWidget(QWidget *parent = 0)
        : QWidget(parent)
    {
        setMinimumHeight(150);
    }

    void setCountry(const CountryGeo &g) { m_geo = g; update(); }
    void setBureaux(const QVector<Bureau> &b) { m_bureaux = b; update(); }

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(), QColor("#bcdcf5"));            // mer

        if (m_geo.contour.isEmpty())
            return;

        // Emprise du pays
        double latMin = 90, latMax = -90, lonMin = 180, lonMax = -180;
        for (int i = 0; i < m_geo.contour.size(); ++i) {
            const QPointF &pt = m_geo.contour.at(i);
            latMin = qMin(latMin, pt.x());
            latMax = qMax(latMax, pt.x());
            lonMin = qMin(lonMin, pt.y());
            lonMax = qMax(lonMax, pt.y());
        }
        const double kx = qCos(qDegreesToRadians((latMin + latMax) / 2.0));
        const double wSpan = (lonMax - lonMin) * kx;
        const double hSpan = latMax - latMin;
        const double scale = qMin((width() - 30) / wSpan, (height() - 30) / hSpan);
        const double offX = (width()  - scale * wSpan) / 2.0;
        const double offY = (height() - scale * hSpan) / 2.0;

        auto proj = [=](double lat, double lon) -> QPointF {
            return QPointF(offX + (lon - lonMin) * kx * scale, offY + (latMax - lat) * scale);
        };

        // Territoire
        QPolygonF land;
        for (int i = 0; i < m_geo.contour.size(); ++i)
            land << proj(m_geo.contour.at(i).x(), m_geo.contour.at(i).y());
        p.setPen(QPen(QColor("#d9d3c0"), 1));
        p.setBrush(QColor("#f3f0e6"));
        p.drawPolygon(land);

        // Bureaux
        QFont f = font();
        f.setPointSize(8);
        p.setFont(f);
        for (int i = 0; i < m_bureaux.size(); ++i) {
            const Bureau &b = m_bureaux.at(i);
            if (!m_geo.coord.contains(b.localisation))
                continue;
            const QPointF ll = m_geo.coord.value(b.localisation);
            QPointF pos = proj(ll.x(), ll.y());
            p.setPen(QPen(Qt::white, 2));
            p.setBrush(b.actif ? QColor("#16a068") : QColor("#e5474f"));
            p.drawEllipse(pos, 7, 7);
            p.setPen(QColor("#334155"));
            p.drawText(pos + QPointF(10, 4), b.localisation);
        }

        // Nom du pays
        QFont fn = font();
        fn.setPointSize(14);
        fn.setBold(true);
        p.setFont(fn);
        p.setPen(QColor(90, 110, 140, 170));
        p.drawText(QPointF(12, height() - 10), m_geo.nom.toUpper());

        // Légende
        QRectF leg(width() - 118, 8, 110, 46);
        p.setPen(QPen(QColor("#d5dbe5")));
        p.setBrush(Qt::white);
        p.drawRoundedRect(leg, 6, 6);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor("#16a068"));
        p.drawEllipse(QPointF(leg.left() + 14, leg.top() + 15), 5, 5);
        p.setBrush(QColor("#e5474f"));
        p.drawEllipse(QPointF(leg.left() + 14, leg.top() + 33), 5, 5);
        QFont fl = font();
        fl.setPointSize(8);
        p.setFont(fl);
        p.setPen(QColor("#334155"));
        p.drawText(QPointF(leg.left() + 26, leg.top() + 19), "Bureau actif");
        p.drawText(QPointF(leg.left() + 26, leg.top() + 37), "Bureau inactif");
    }

private:
    CountryGeo m_geo;
    QVector<Bureau> m_bureaux;
};

/* ------------------------------------------------------------------ */
/*  Fenêtre principale : Gestion des bureaux                          */
/* ------------------------------------------------------------------ */
class GBureaux : public QMainWindow
{
    Q_OBJECT

public:
    explicit GBureaux(QWidget *parent = 0);

private slots:
    void onAdd();            // Ajouter
    void onEdit();           // Modifier
    void onDelete();         // Supprimer
    void onClear();          // Vider le formulaire
    void onSort();           // Trier
    void onSearch();         // Rechercher (ID ou statut)
    void onRowSelected();    // ligne sélectionnée -> remplit le formulaire
    void updateClock();      // date et heure de l'en-tête
    void exportPdf();

private:
    // Construction de l'interface
    QWidget *buildSidebar();
    QWidget *buildHeader();
    QWidget *buildFormCard();
    QWidget *buildToolbar();
    QWidget *buildTableCard();
    QWidget *buildStatsCard();
    QWidget *buildMapCard();
    static QString styleSheet();

    // Logique
    void detectCountry();
    void loadSampleData();
    void refreshTable();
    void updateStats();
    bool readForm(Bureau &b);
    QString nextId() const;

    QVector<Bureau> m_bureaux;
    QString m_editId;            // ID du bureau sélectionné ("" = aucun)
    int m_sortMode;              // -1 : ordre d'ajout, 0 : responsable, 1 : date
    CountryGeo m_geo;
    QString m_geoNote;

    // Widgets
    QTableWidget *m_table;
    QLineEdit    *m_search;
    QComboBox    *m_sort;
    DonutChart   *m_donut;
    QLabel       *m_actifsLabel;
    QLabel       *m_inactifsLabel;
    MapWidget    *m_map;
    QLabel       *m_dateLabel;
    QLabel       *m_timeLabel;

    QLineEdit    *m_id;
    QLineEdit    *m_nom;
    QLineEdit    *m_prenom;
    QComboBox    *m_loc;
    QDateEdit    *m_date;
    QComboBox    *m_statut;
};

#endif // GBUREAUX_H