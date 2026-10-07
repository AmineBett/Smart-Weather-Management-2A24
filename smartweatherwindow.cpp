#include "smartweatherwindow.h"

#include "stationwindow.h"
#include "technicienne.h"

#include <QPushButton>
#include <QStackedWidget>

SmartWeatherWindow::SmartWeatherWindow(QWidget *parent)
    : QMainWindow(parent),
      stack(new QStackedWidget(this)),
      stationWindow(new StationWindow),
      technicienneWindow(new Technicienne)
{
    setWindowTitle("SmartWeather");
    resize(1450, 850);
    setMinimumSize(1200, 700);

    setCentralWidget(stack);

    stack->addWidget(stationWindow);
    stack->addWidget(technicienneWindow);

    QPushButton *techniciansButton =
        findNavigationButton(stationWindow, "Gestion des techniciens");

    if (techniciansButton)
    {
        connect(
            techniciansButton,
            &QPushButton::clicked,
            this,
            [this]()
            {
                stack->setCurrentWidget(technicienneWindow);
            }
            );
    }

    QPushButton *stationsButton =
        findNavigationButton(technicienneWindow, "Gestion des stations");

    if (stationsButton)
    {
        connect(
            stationsButton,
            &QPushButton::clicked,
            this,
            [this]()
            {
                stack->setCurrentWidget(stationWindow);
            }
            );
    }
}

QPushButton *SmartWeatherWindow::findNavigationButton(QWidget *page,
                                                      const QString &text) const
{
    const QList<QPushButton *> buttons =
        page->findChildren<QPushButton *>();

    for (QPushButton *button : buttons)
    {
        if (button->text().contains(text))
        {
            return button;
        }
    }

    return nullptr;
}
