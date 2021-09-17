#pragma once

#include <QtWidgets>

#include "app/widgets/dialog.hpp"
#include "canbus/psaserial.hpp"

class MessageSnackBar : public SnackBar {
    Q_OBJECT
    public:
        MessageSnackBar(Arbiter &arbiter);
        void setMessage(int message);
        void openMessage(int message, bool show);
    private:
        QWidget *body_widget();
        QWidget *state_widget();
        QLabel *msg;
};
