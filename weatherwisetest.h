#ifndef WEATHERWISETEST_H
#define WEATHERWISETEST_H

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

/* ------------------------------------------------------------------ */
/* Structure de données : Equipement                                  */
/* ------------------------------------------------------------------ */

struct Equipement
{
    QString id;
    QString nom;
    QString localisation;
    QDate dateInstallation;
    QString statut;
    QString type;

    QString description() const
    {
        return nom + " - " + localisation;
    }
};

/* ------------------------------------------------------------------ */
/* Données géographiques                                               */
/* ------------------------------------------------------------------ */

struct CountryGeo
{
    QString nom;
    QVector<QPointF> contour;
    QStringList villes;
    QHash<QString, QPointF> coord;

    void addVille(const QString &n, double lat, double lon)
    {
        villes << n;
        coord.insert(n, QPointF(lat, lon));
    }
};

/* ------------------------------------------------------------------ */
/* Graphique en anneau                                                 */
/* ------------------------------------------------------------------ */

class DonutChart : public QWidget
{
public:
    explicit DonutChart(QWidget *parent = 0)
        : QWidget(parent),
        m_actifs(0),
        m_inactifs(0)
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

        QRectF r(
            (width() - side) / 2.0,
            (height() - side) / 2.0,
            side,
            side
            );

        r.adjust(
            penW / 2,
            penW / 2,
            -penW / 2,
            -penW / 2
            );

        QPen pen;
        pen.setWidthF(penW);
        pen.setCapStyle(Qt::FlatCap);

        if (total == 0)
        {
            pen.setColor(QColor("#e5e7eb"));
            p.setPen(pen);
            p.drawArc(r, 0, 360 * 16);
        }
        else
        {
            const int start = 90 * 16;

            const int spanA =
                -static_cast<int>(
                    360.0 * 16.0 * m_actifs / total
                    );

            const int spanI =
                -360 * 16 - spanA;

            pen.setColor(QColor("#16a068"));
            p.setPen(pen);
            p.drawArc(r, start, spanA);

            if (m_inactifs > 0)
            {
                pen.setColor(QColor("#e5474f"));
                p.setPen(pen);
                p.drawArc(
                    r,
                    start + spanA,
                    spanI
                    );
            }
        }

        p.setPen(QColor("#0f2a52"));

        QFont f = font();
        f.setPointSize(22);
        f.setBold(true);
        p.setFont(f);

        p.drawText(
            QRectF(
                0,
                height() / 2.0 - 30,
                width(),
                32
                ),
            Qt::AlignCenter,
            QString::number(total)
            );

        f.setPointSize(10);
        f.setBold(false);
        p.setFont(f);

        p.drawText(
            QRectF(
                0,
                height() / 2.0 + 2,
                width(),
                20
                ),
            Qt::AlignCenter,
            "équipements"
            );
    }

private:
    int m_actifs;
    int m_inactifs;
};

/* ------------------------------------------------------------------ */
/* Carte géographique                                                  */
/* ------------------------------------------------------------------ */

class MapWidget : public QWidget
{
public:
    explicit MapWidget(QWidget *parent = 0)
        : QWidget(parent)
    {
        setMinimumHeight(150);
    }

    void setCountry(const CountryGeo &g)
    {
        m_geo = g;
        update();
    }

    void setEquipements(const QVector<Equipement> &e)
    {
        m_equipements = e;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        p.fillRect(
            rect(),
            QColor("#bcdcf5")
            );

        if (m_geo.contour.isEmpty())
            return;

        double latMin = 90;
        double latMax = -90;
        double lonMin = 180;
        double lonMax = -180;

        for (int i = 0; i < m_geo.contour.size(); ++i)
        {
            const QPointF &pt = m_geo.contour.at(i);

            latMin = qMin(latMin, pt.x());
            latMax = qMax(latMax, pt.x());
            lonMin = qMin(lonMin, pt.y());
            lonMax = qMax(lonMax, pt.y());
        }

        const double kx =
            qCos(
                qDegreesToRadians(
                    (latMin + latMax) / 2.0
                    )
                );

        const double wSpan =
            (lonMax - lonMin) * kx;

        const double hSpan =
            latMax - latMin;

        const double scale =
            qMin(
                (width() - 30) / wSpan,
                (height() - 30) / hSpan
                );

        const double offX =
            (width() - scale * wSpan) / 2.0;

        const double offY =
            (height() - scale * hSpan) / 2.0;

        /*
         * Projection latitude / longitude
         */
        const double currentScale = scale;

        /*
         * Fonction de projection compatible C++11
         */
        auto proj =
            [=](double lat, double lon) -> QPointF
        {
            return QPointF(
                offX +
                    (lon - lonMin) *
                        kx *
                        currentScale,

                offY +
                    (latMax - lat) *
                        currentScale
                );
        };

        /* Territoire */
        QPolygonF land;

        for (int i = 0; i < m_geo.contour.size(); ++i)
        {
            const QPointF pt =
                m_geo.contour.at(i);

            land <<
                proj(
                    pt.x(),
                    pt.y()
                    );
        }

        p.setPen(
            QPen(
                QColor("#d9d3c0"),
                1
                )
            );

        p.setBrush(
            QColor("#f3f0e6")
            );

        p.drawPolygon(land);

        /* Equipements */
        QFont f = font();
        f.setPointSize(8);
        p.setFont(f);

        for (int i = 0;
             i < m_equipements.size();
             ++i)
        {
            const Equipement &e =
                m_equipements.at(i);

            if (!m_geo.coord.contains(e.localisation))
                continue;

            const QPointF ll =
                m_geo.coord.value(
                    e.localisation
                    );

            QPointF pos =
                proj(
                    ll.x(),
                    ll.y()
                    );

            QColor markerColor;

            if (e.statut == "Actif")
                markerColor = QColor("#16a068");
            else if (e.statut == "Inactif")
                markerColor = QColor("#e5474f");
            else
                markerColor = QColor("#f0a000");

            p.setPen(
                QPen(
                    Qt::white,
                    2
                    )
                );

            p.setBrush(markerColor);

            p.drawEllipse(
                pos,
                7,
                7
                );

            p.setPen(
                QColor("#334155")
                );

            p.drawText(
                pos + QPointF(10, 4),
                e.localisation
                );
        }

        /* Nom du pays */
        QFont fn = font();
        fn.setPointSize(14);
        fn.setBold(true);
        p.setFont(fn);

        p.setPen(
            QColor(90, 110, 140, 170)
            );

        p.drawText(
            QPointF(
                12,
                height() - 10
                ),
            m_geo.nom.toUpper()
            );

        /* Légende */
        QRectF leg(
            width() - 145,
            8,
            137,
            64
            );

        p.setPen(
            QPen(
                QColor("#d5dbe5")
                )
            );

        p.setBrush(Qt::white);

        p.drawRoundedRect(
            leg,
            6,
            6
            );

        p.setPen(Qt::NoPen);

        p.setBrush(
            QColor("#16a068")
            );

        p.drawEllipse(
            QPointF(
                leg.left() + 14,
                leg.top() + 15
                ),
            5,
            5
            );

        p.setBrush(
            QColor("#e5474f")
            );

        p.drawEllipse(
            QPointF(
                leg.left() + 14,
                leg.top() + 33
                ),
            5,
            5
            );

        p.setBrush(
            QColor("#f0a000")
            );

        p.drawEllipse(
            QPointF(
                leg.left() + 14,
                leg.top() + 51
                ),
            5,
            5
            );

        QFont fl = font();
        fl.setPointSize(8);
        p.setFont(fl);

        p.setPen(
            QColor("#334155")
            );

        p.drawText(
            QPointF(
                leg.left() + 26,
                leg.top() + 19
                ),
            "Équipement actif"
            );

        p.drawText(
            QPointF(
                leg.left() + 26,
                leg.top() + 37
                ),
            "Équipement inactif"
            );

        p.drawText(
            QPointF(
                leg.left() + 26,
                leg.top() + 55
                ),
            "En maintenance"
            );
    }

private:
    CountryGeo m_geo;
    QVector<Equipement> m_equipements;
};

/* ------------------------------------------------------------------ */
/* Fenêtre principale                                                  */
/* ------------------------------------------------------------------ */

class WeatherWiseTest : public QMainWindow
{
    Q_OBJECT

public:
    explicit WeatherWiseTest(QWidget *parent = 0);

private slots:

    void onAdd();
    void onEdit();
    void onDelete();
    void onClear();

    void onSort();
    void onSearch();

    void onRowSelected();

    void updateClock();

    void exportPdf();

private:

    /* Interface */
    QWidget *buildSidebar();
    QWidget *buildHeader();
    QWidget *buildFormCard();
    QWidget *buildToolbar();
    QWidget *buildTableCard();
    QWidget *buildStatsCard();
    QWidget *buildMapCard();

    static QString styleSheet();

    /* Logique */
    void detectCountry();
    void loadSampleData();
    void refreshTable();
    void updateStats();

    bool readForm(Equipement &e);

    QString nextId() const;

private:

    QVector<Equipement> m_equipements;

    QString m_editId;

    /*
     * -1 = ordre d'ajout
     *  0 = nom
     *  1 = localisation
     *  2 = statut
     *  3 = type
     */
    int m_sortMode;

    CountryGeo m_geo;

    QString m_geoNote;

    /* Widgets */

    QTableWidget *m_table;

    QLineEdit *m_search;

    QComboBox *m_sort;

    DonutChart *m_donut;

    QLabel *m_actifsLabel;
    QLabel *m_inactifsLabel;
    QLabel *m_maintenanceLabel;

    MapWidget *m_map;

    QLabel *m_dateLabel;
    QLabel *m_timeLabel;

    /* Formulaire */

    QLineEdit *m_id;
    QLineEdit *m_nom;

    QComboBox *m_loc;

    QDateEdit *m_date;

    QComboBox *m_statut;

    QComboBox *m_type;
};

#endif // WEATHERWISETEST_H