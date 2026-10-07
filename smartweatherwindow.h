#ifndef SMARTWEATHERWINDOW_H
#define SMARTWEATHERWINDOW_H

#include <QMainWindow>

class QPushButton;
class QFrame;
class QStackedWidget;
class QString;
class QWidget;
class GBureaux;
class StationWindow;
class Technicienne;
class WeatherWiseTest;

class SmartWeatherWindow : public QMainWindow
{
public:
    explicit SmartWeatherWindow(QWidget *parent = nullptr);

private:
    QFrame *createSidebar();
    QPushButton *createNavigationButton(const QString &text);
    void preparePage(QWidget *page);
    void showPage(QWidget *page, QPushButton *activeButton);
    void updateActiveButton(QPushButton *activeButton);

    QStackedWidget *stack;
    QPushButton *bureauButton;
    QPushButton *equipmentButton;
    QPushButton *stationsButton;
    QPushButton *techniciensButton;
    StationWindow *stationWindow;
    Technicienne *technicienneWindow;
    GBureaux *bureauxWindow;
    WeatherWiseTest *equipmentWindow;
};

#endif // SMARTWEATHERWINDOW_H
