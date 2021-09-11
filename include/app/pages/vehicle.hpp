#pragma once

#include <QFileInfo>
#include <QMap>
#include <QMediaPlayer>
#include <QPluginLoader>
#include <QString>
#include <QtWidgets>
#include <QDebug>
#include "app/config.hpp"
#include "app/pages/page.hpp"
#include "app/widgets/selector.hpp"
#include "canbus/psaserial.hpp"

class Arbiter;

class VehiclePage : public QWidget, public Page {
    Q_OBJECT
   private:

   public:
    VehiclePage(Arbiter &arbiter, QWidget *parent = nullptr);

    void init() override;

};
