/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QSlider>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QLabel *inputLabel;
    QListView *listView;
    QLabel *inputDeviceLabel;
    QComboBox *outputDeviceCombobox;
    QLabel *opVolumeLabel;
    QLabel *opIconlabel;
    QLabel *opVolumePercentLabel;
    QSlider *opVolumeSlider;
    QLabel *opBalanceLabel;
    QLabel *leftBalanceLabel;
    QLabel *rightBalanceLabel;
    QSlider *opBalanceSlider;
    QLabel *OutputLabel;
    QListView *listView_2;
    QLabel *outputDeviceLabel;
    QComboBox *inputDeviceCombobox;
    QLabel *ipIconlabel;
    QSlider *inputLevelSlider;
    QLabel *lowLevelLabel;
    QLabel *ipVolumeLabel;
    QLabel *inputLevelLabel;
    QLabel *highLevelLabel;
    QLabel *ipVolumePercentLabel;
    QSlider *ipVolumeSlider;
    QLabel *soundEffectLabel;
    QLabel *soundThemeLabel;
    QComboBox *soundThemeCombobox;
    QListView *listView_3;
    QLabel *shutdownlabel;
    QComboBox *shutdownCombobox;
    QLabel *lagoutLabel;
    QComboBox *lagoutCombobox;
    QLabel *applicationLabel;
    QTableView *appVolumeTableView;
    QLabel *label;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(942, 990);
        inputLabel = new QLabel(Widget);
        inputLabel->setObjectName(QString::fromUtf8("inputLabel"));
        inputLabel->setGeometry(QRect(10, 210, 59, 20));
        listView = new QListView(Widget);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setGeometry(QRect(10, 40, 611, 151));
        inputDeviceLabel = new QLabel(Widget);
        inputDeviceLabel->setObjectName(QString::fromUtf8("inputDeviceLabel"));
        inputDeviceLabel->setGeometry(QRect(10, 50, 141, 20));
        outputDeviceCombobox = new QComboBox(Widget);
        outputDeviceCombobox->setObjectName(QString::fromUtf8("outputDeviceCombobox"));
        outputDeviceCombobox->setGeometry(QRect(300, 40, 71, 30));
        opVolumeLabel = new QLabel(Widget);
        opVolumeLabel->setObjectName(QString::fromUtf8("opVolumeLabel"));
        opVolumeLabel->setGeometry(QRect(10, 80, 59, 20));
        opIconlabel = new QLabel(Widget);
        opIconlabel->setObjectName(QString::fromUtf8("opIconlabel"));
        opIconlabel->setGeometry(QRect(80, 80, 59, 20));
        opVolumePercentLabel = new QLabel(Widget);
        opVolumePercentLabel->setObjectName(QString::fromUtf8("opVolumePercentLabel"));
        opVolumePercentLabel->setGeometry(QRect(320, 80, 59, 20));
        opVolumeSlider = new QSlider(Widget);
        opVolumeSlider->setObjectName(QString::fromUtf8("opVolumeSlider"));
        opVolumeSlider->setGeometry(QRect(140, 80, 160, 21));
        opVolumeSlider->setOrientation(Qt::Horizontal);
        opBalanceLabel = new QLabel(Widget);
        opBalanceLabel->setObjectName(QString::fromUtf8("opBalanceLabel"));
        opBalanceLabel->setGeometry(QRect(10, 120, 59, 20));
        leftBalanceLabel = new QLabel(Widget);
        leftBalanceLabel->setObjectName(QString::fromUtf8("leftBalanceLabel"));
        leftBalanceLabel->setGeometry(QRect(80, 120, 59, 20));
        rightBalanceLabel = new QLabel(Widget);
        rightBalanceLabel->setObjectName(QString::fromUtf8("rightBalanceLabel"));
        rightBalanceLabel->setGeometry(QRect(350, 130, 59, 16));
        opBalanceSlider = new QSlider(Widget);
        opBalanceSlider->setObjectName(QString::fromUtf8("opBalanceSlider"));
        opBalanceSlider->setGeometry(QRect(150, 130, 160, 21));
        opBalanceSlider->setOrientation(Qt::Horizontal);
        OutputLabel = new QLabel(Widget);
        OutputLabel->setObjectName(QString::fromUtf8("OutputLabel"));
        OutputLabel->setGeometry(QRect(10, 10, 59, 20));
        listView_2 = new QListView(Widget);
        listView_2->setObjectName(QString::fromUtf8("listView_2"));
        listView_2->setGeometry(QRect(10, 230, 641, 131));
        outputDeviceLabel = new QLabel(Widget);
        outputDeviceLabel->setObjectName(QString::fromUtf8("outputDeviceLabel"));
        outputDeviceLabel->setGeometry(QRect(20, 240, 121, 20));
        inputDeviceCombobox = new QComboBox(Widget);
        inputDeviceCombobox->setObjectName(QString::fromUtf8("inputDeviceCombobox"));
        inputDeviceCombobox->setGeometry(QRect(300, 230, 71, 30));
        ipIconlabel = new QLabel(Widget);
        ipIconlabel->setObjectName(QString::fromUtf8("ipIconlabel"));
        ipIconlabel->setGeometry(QRect(90, 270, 59, 20));
        inputLevelSlider = new QSlider(Widget);
        inputLevelSlider->setObjectName(QString::fromUtf8("inputLevelSlider"));
        inputLevelSlider->setGeometry(QRect(150, 310, 160, 21));
        inputLevelSlider->setOrientation(Qt::Horizontal);
        lowLevelLabel = new QLabel(Widget);
        lowLevelLabel->setObjectName(QString::fromUtf8("lowLevelLabel"));
        lowLevelLabel->setGeometry(QRect(90, 310, 59, 20));
        ipVolumeLabel = new QLabel(Widget);
        ipVolumeLabel->setObjectName(QString::fromUtf8("ipVolumeLabel"));
        ipVolumeLabel->setGeometry(QRect(20, 270, 59, 20));
        inputLevelLabel = new QLabel(Widget);
        inputLevelLabel->setObjectName(QString::fromUtf8("inputLevelLabel"));
        inputLevelLabel->setGeometry(QRect(20, 310, 59, 20));
        highLevelLabel = new QLabel(Widget);
        highLevelLabel->setObjectName(QString::fromUtf8("highLevelLabel"));
        highLevelLabel->setGeometry(QRect(330, 310, 59, 16));
        ipVolumePercentLabel = new QLabel(Widget);
        ipVolumePercentLabel->setObjectName(QString::fromUtf8("ipVolumePercentLabel"));
        ipVolumePercentLabel->setGeometry(QRect(340, 270, 59, 20));
        ipVolumeSlider = new QSlider(Widget);
        ipVolumeSlider->setObjectName(QString::fromUtf8("ipVolumeSlider"));
        ipVolumeSlider->setGeometry(QRect(150, 270, 160, 21));
        ipVolumeSlider->setOrientation(Qt::Horizontal);
        soundEffectLabel = new QLabel(Widget);
        soundEffectLabel->setObjectName(QString::fromUtf8("soundEffectLabel"));
        soundEffectLabel->setGeometry(QRect(20, 380, 141, 20));
        soundThemeLabel = new QLabel(Widget);
        soundThemeLabel->setObjectName(QString::fromUtf8("soundThemeLabel"));
        soundThemeLabel->setGeometry(QRect(20, 410, 131, 20));
        soundThemeCombobox = new QComboBox(Widget);
        soundThemeCombobox->setObjectName(QString::fromUtf8("soundThemeCombobox"));
        soundThemeCombobox->setGeometry(QRect(260, 410, 71, 30));
        listView_3 = new QListView(Widget);
        listView_3->setObjectName(QString::fromUtf8("listView_3"));
        listView_3->setGeometry(QRect(10, 440, 641, 121));
        shutdownlabel = new QLabel(Widget);
        shutdownlabel->setObjectName(QString::fromUtf8("shutdownlabel"));
        shutdownlabel->setGeometry(QRect(30, 460, 59, 20));
        shutdownCombobox = new QComboBox(Widget);
        shutdownCombobox->setObjectName(QString::fromUtf8("shutdownCombobox"));
        shutdownCombobox->setGeometry(QRect(360, 460, 71, 30));
        lagoutLabel = new QLabel(Widget);
        lagoutLabel->setObjectName(QString::fromUtf8("lagoutLabel"));
        lagoutLabel->setGeometry(QRect(30, 500, 59, 20));
        lagoutCombobox = new QComboBox(Widget);
        lagoutCombobox->setObjectName(QString::fromUtf8("lagoutCombobox"));
        lagoutCombobox->setGeometry(QRect(360, 500, 71, 30));
        applicationLabel = new QLabel(Widget);
        applicationLabel->setObjectName(QString::fromUtf8("applicationLabel"));
        applicationLabel->setGeometry(QRect(20, 580, 131, 20));
        appVolumeTableView = new QTableView(Widget);
        appVolumeTableView->setObjectName(QString::fromUtf8("appVolumeTableView"));
        appVolumeTableView->setGeometry(QRect(10, 610, 651, 192));
        label = new QLabel(Widget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 830, 59, 20));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "Widget", nullptr));
        inputLabel->setText(QApplication::translate("Widget", "Input", nullptr));
        inputDeviceLabel->setText(QApplication::translate("Widget", "Select output device", nullptr));
        opVolumeLabel->setText(QApplication::translate("Widget", "Volume", nullptr));
        opIconlabel->setText(QApplication::translate("Widget", "icon", nullptr));
        opVolumePercentLabel->setText(QApplication::translate("Widget", "0%", nullptr));
        opBalanceLabel->setText(QApplication::translate("Widget", "Channel balance", nullptr));
        leftBalanceLabel->setText(QApplication::translate("Widget", "left", nullptr));
        rightBalanceLabel->setText(QApplication::translate("Widget", "right", nullptr));
        OutputLabel->setText(QApplication::translate("Widget", "output", nullptr));
        outputDeviceLabel->setText(QApplication::translate("Widget", "Select input device", nullptr));
        ipIconlabel->setText(QApplication::translate("Widget", "icon", nullptr));
        lowLevelLabel->setText(QApplication::translate("Widget", "low", nullptr));
        ipVolumeLabel->setText(QApplication::translate("Widget", "Volume", nullptr));
        inputLevelLabel->setText(QApplication::translate("Widget", "Input level", nullptr));
        highLevelLabel->setText(QApplication::translate("Widget", "high", nullptr));
        ipVolumePercentLabel->setText(QApplication::translate("Widget", "0%", nullptr));
        soundEffectLabel->setText(QApplication::translate("Widget", "System sound effects", nullptr));
        soundThemeLabel->setText(QApplication::translate("Widget", "System sound theme", nullptr));
        shutdownlabel->setText(QApplication::translate("Widget", "shutdown", nullptr));
        lagoutLabel->setText(QApplication::translate("Widget", "Lagout", nullptr));
        applicationLabel->setText(QApplication::translate("Widget", "Application Volume", nullptr));
        label->setText(QApplication::translate("Widget", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
