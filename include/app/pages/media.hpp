#pragma once

#include <QFileInfo>
#include <QMap>
#include <QMediaPlayer>
#include <QPluginLoader>
#include <QString>
#include <QtWidgets>

#include "app/config.hpp"
#include "app/pages/page.hpp"
#include "app/widgets/selector.hpp"
#include "app/widgets/tuner.hpp"
#include "canbus/psaserial.hpp"
#include "app/widgets/volume.hpp"
#include "app/widgets/dialog.hpp"

class Arbiter;

class MediaPage : public QTabWidget, public Page {
    Q_OBJECT
   private:

   public:
    MediaPage(Arbiter &arbiter, QWidget *parent = nullptr);

    class Settings : public QWidget {
       public:
        Settings(Arbiter &arbiter, QWidget *parent = nullptr);

       protected:
        QSize sizeHint() const override;

       private:
        QLayout *settings_widget();
        QLayout *bass_widget();
        QLayout *treble_widget();
        QLayout *bal_widget();
        QLayout *fad_widget();
        QLayout *eq_widget();
        QLayout *loud_widget();
        QLayout *autoSound_widget();

        Arbiter &arbiter;
    };

    void init() override;
    QWidget *setting_menu();

};


class RadioPlayerTab : public QWidget {
    Q_OBJECT

   public:
    RadioPlayerTab(Arbiter &arbiter, QWidget *parent = nullptr);
    ~RadioPlayerTab();



   private:
    static QMap<QString, QFileInfo> get_plugins();

    Arbiter &arbiter;
    Config *config;
    //QMap<QString, QFileInfo> plugins;
    QPluginLoader loader;
    Tuner *tuner;
    Selector *plugin_selector;
    QPushButton *play_button;

    QLabel *radioName;
   QLabel *frequency;
   QLabel *Hz;
   QLabel *band;
   QLabel *bank;


    void load_plugin();
    QWidget *dialog_body();
    QWidget *tuner_widget();
    QWidget *tuner_test();
    QWidget *controls_widget();

};

//CD player tab copy paste to USB as well.
class CDPlayerTab : public QWidget {
    Q_OBJECT

   public:
    CDPlayerTab(Arbiter &arbiter, QWidget *parent = nullptr);

    static QString durationFmt(int total_ms);

   private:
    Arbiter &arbiter;

    QWidget *track_widget();
    QWidget *controls_widget();
};

class USBPlayerTab : public QWidget {
    Q_OBJECT

   public:
    USBPlayerTab(Arbiter &arbiter, QWidget *parent = nullptr);

    static QString durationFmt(int total_ms);

   private:
    Arbiter &arbiter;

    QWidget *track_widget();
    QWidget *controls_widget();
};
