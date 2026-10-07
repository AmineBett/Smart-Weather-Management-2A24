#ifndef SMARTWEATHERWINDOW_H
#define SMARTWEATHERWINDOW_H

#include <QMainWindow>

class QPushButton;
class QStackedWidget;
class QString;
class QWidget;
class StationWindow;
class Technicienne;

class SmartWeatherWindow : public QMainWindow
{
public:
    explicit SmartWeatherWindow(QWidget *parent = nullptr);

private:
    QPushButton *findNavigationButton(QWidget *page,
                                      const QString &text) const;

    QStackedWidget *stack;
    StationWindow *stationWindow;
    Technicienne *technicienneWindow;
};

#endif // SMARTWEATHERWINDOW_H
