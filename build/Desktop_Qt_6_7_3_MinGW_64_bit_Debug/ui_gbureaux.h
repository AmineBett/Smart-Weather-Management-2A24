/********************************************************************************
** Form generated from reading UI file 'gbureaux.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GBUREAUX_H
#define UI_GBUREAUX_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GBureaux
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *GBureaux)
    {
        if (GBureaux->objectName().isEmpty())
            GBureaux->setObjectName("GBureaux");
        GBureaux->resize(800, 600);
        centralwidget = new QWidget(GBureaux);
        centralwidget->setObjectName("centralwidget");
        GBureaux->setCentralWidget(centralwidget);
        menubar = new QMenuBar(GBureaux);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 25));
        GBureaux->setMenuBar(menubar);
        statusbar = new QStatusBar(GBureaux);
        statusbar->setObjectName("statusbar");
        GBureaux->setStatusBar(statusbar);

        retranslateUi(GBureaux);

        QMetaObject::connectSlotsByName(GBureaux);
    } // setupUi

    void retranslateUi(QMainWindow *GBureaux)
    {
        GBureaux->setWindowTitle(QCoreApplication::translate("GBureaux", "GBureaux", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GBureaux: public Ui_GBureaux {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GBUREAUX_H
