#pragma once

#include <QButtonGroup>
#include <QKeyEvent>
#include <QMainWindow>
#include <QObject>
#include <QShowEvent>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QWidget>

#include "app/config.hpp"
#include "app/pages/openauto.hpp"
#include "app/pages/page.hpp"
#include "app/widgets/volume.hpp"
#include "app/arbiter.hpp"
#include "app/widgets/messages.hpp"

class Starlight : public QWidget {
    Q_OBJECT

   public:
    Starlight(Arbiter &arbiter);
    void init();

   private:
    struct NavRail {
        QButtonGroup group;
        QVBoxLayout *layout;

        NavRail();
    };

    struct Body {
        QVBoxLayout *layout;
        QStackedLayout *frame;

        Body();
    };

    Arbiter &arbiter;
    NavRail rail;
    Body body;
    void init_connected_pages();
    VolumeSnackBar *volume;
    int oldVol = 0;
    MessageSnackBar *msg;
    bool oldMsg = false;
    void set_page(Page *page);
    QWidget *control_bar() const;
    QWidget *power_control() const;
};

class Window : public QMainWindow {
    Q_OBJECT

   public:
    Window();

   protected:
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

   private:
    Arbiter arbiter;
};
