#include "smartweatherwindow.h"

#include "gbureaux.h"
#include "stationwindow.h"
#include "technicienne.h"
#include "weatherwisetest.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QStyle>
#include <QVBoxLayout>

SmartWeatherWindow::SmartWeatherWindow(QWidget *parent)
    : QMainWindow(parent),
      stack(new QStackedWidget(this)),
      bureauButton(nullptr),
      equipmentButton(nullptr),
      stationsButton(nullptr),
      techniciensButton(nullptr),
      stationWindow(new StationWindow),
      technicienneWindow(new Technicienne),
      bureauxWindow(new GBureaux),
      equipmentWindow(new WeatherWiseTest)
{
    setWindowTitle("SmartWeather");
    resize(1450, 850);
    setMinimumSize(1200, 700);

    QWidget *central =
        new QWidget(this);

    QHBoxLayout *layout =
        new QHBoxLayout(central);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(createSidebar());
    layout->addWidget(stack, 1);

    setCentralWidget(central);

    preparePage(stationWindow);
    preparePage(technicienneWindow);
    preparePage(bureauxWindow);
    preparePage(equipmentWindow);

    stack->addWidget(stationWindow);
    stack->addWidget(technicienneWindow);
    stack->addWidget(bureauxWindow);
    stack->addWidget(equipmentWindow);

    showPage(stationWindow, stationsButton);
}

QFrame *SmartWeatherWindow::createSidebar()
{
    QFrame *sidebar =
        new QFrame(this);

    sidebar->setObjectName("mainSidebar");
    sidebar->setFixedWidth(285);

    QVBoxLayout *layout =
        new QVBoxLayout(sidebar);

    layout->setContentsMargins(8, 22, 8, 16);
    layout->setSpacing(8);

    QLabel *logo =
        new QLabel("*", sidebar);

    logo->setObjectName("mainLogo");
    logo->setAlignment(Qt::AlignCenter);

    QLabel *title =
        new QLabel("SMART WEATHER\nMANAGEMENT", sidebar);

    title->setObjectName("mainTitle");
    title->setAlignment(Qt::AlignCenter);

    layout->addWidget(logo);
    layout->addWidget(title);
    layout->addSpacing(22);

    QPushButton *homeButton =
        createNavigationButton("Accueil");

    bureauButton =
        createNavigationButton("Gestion de bureau");

    equipmentButton =
        createNavigationButton("Gestion des equipements");

    techniciensButton =
        createNavigationButton("Gestion des techniciens");

    stationsButton =
        createNavigationButton("Gestion des stations");

    QPushButton *usersButton =
        createNavigationButton("Gestion des utilisateurs");

    layout->addWidget(homeButton);
    layout->addWidget(bureauButton);
    layout->addWidget(equipmentButton);
    layout->addWidget(techniciensButton);
    layout->addWidget(stationsButton);
    layout->addWidget(usersButton);
    layout->addStretch();

    connect(
        bureauButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showPage(bureauxWindow, bureauButton);
        }
        );

    connect(
        equipmentButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showPage(equipmentWindow, equipmentButton);
        }
        );

    connect(
        stationsButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showPage(stationWindow, stationsButton);
        }
        );

    connect(
        techniciensButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showPage(technicienneWindow, techniciensButton);
        }
        );

    sidebar->setStyleSheet(R"(
        #mainSidebar {
            background: #17467D;
        }

        #mainLogo {
            color: #FFD21F;
            font-size: 48px;
            font-weight: bold;
            background: transparent;
        }

        #mainTitle {
            color: white;
            font-size: 18px;
            font-weight: bold;
            background: transparent;
        }

        QPushButton#navButton {
            min-height: 48px;
            border: none;
            border-radius: 7px;
            padding-left: 22px;
            text-align: left;
            color: white;
            background: transparent;
            font-size: 14px;
        }

        QPushButton#navButton:hover {
            background: #2468B4;
        }

        QPushButton#navButton[active="true"] {
            background: #2F83D8;
            font-weight: bold;
        }
    )");

    return sidebar;
}

QPushButton *SmartWeatherWindow::createNavigationButton(const QString &text)
{
    QPushButton *button =
        new QPushButton(text, this);

    button->setObjectName("navButton");
    button->setProperty("active", false);
    button->setCursor(Qt::PointingHandCursor);

    return button;
}

void SmartWeatherWindow::preparePage(QWidget *page)
{
    QWidget *sidebar =
        page->findChild<QWidget *>("sidebar");

    if (sidebar)
    {
        sidebar->hide();
        sidebar->setFixedWidth(0);
    }
}

void SmartWeatherWindow::showPage(QWidget *page, QPushButton *activeButton)
{
    stack->setCurrentWidget(page);
    updateActiveButton(activeButton);
}

void SmartWeatherWindow::updateActiveButton(QPushButton *activeButton)
{
    QPushButton *buttons[] = {
        bureauButton,
        equipmentButton,
        stationsButton,
        techniciensButton
    };

    for (QPushButton *button : buttons)
    {
        if (!button)
            continue;

        button->setProperty("active", button == activeButton);
        button->style()->unpolish(button);
        button->style()->polish(button);
        button->update();
    }
}
