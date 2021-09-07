#pragma once

#include <QtWidgets>

#include "app/widgets/dialog.hpp"
#include "canbus/psaserial.hpp"

class VolumeSnackBar : public SnackBar {
    Q_OBJECT
    public:
        VolumeSnackBar(Arbiter &arbiter);
        void setVolume(int volume);
        void openVolume(int volume);
    private:
        QWidget *body_widget();
        QWidget *state_widget();
        QLabel *vol;
        QSlider *slider;
};
