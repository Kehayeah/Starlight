#include <fileref.h>
#include <math.h>
#include <tag.h>
#include <tpropertymap.h>
#include <QDirIterator>
#include <QListWidget>
#include <QListWidgetItem>

#include "app/window.hpp"
#include "app/pages/vehicle.hpp"

VehiclePage::VehiclePage(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    , Page(arbiter, "Vehicle", "speed", true, this)
{
}

void VehiclePage::init()
{
    auto *layout = new QVBoxLayout(this);
    auto *horLay = new QGridLayout();
    //auto *fuel = new QVBoxLayout();
    //auto *fuelNow = new QVBoxLayout();

    auto *label = new QLabel("Trip Info");

    auto *spaceLabel = new QLabel(" ");
    auto *kmLeft = new QLabel();
    kmLeft->setPixmap(QPixmap(":/icons/gasoline-pump.svg").scaled(100,100,Qt::KeepAspectRatio));
    kmLeft->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    auto *kmLeftNum = new QLabel("150 Km");
    kmLeftNum->setStyleSheet("QLabel {font-size: 25pt;}");

    auto *fuelCurr = new QLabel();
    fuelCurr->setPixmap(QPixmap(":/icons/fuel.svg").scaled(100,100,Qt::KeepAspectRatio));
    fuelCurr->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    auto *fuelCurrNum = new QLabel("7.0");
    fuelCurrNum->setStyleSheet("QLabel {font-size: 25pt;}");

    auto *rpmGauge = new QLabel();
    rpmGauge->setPixmap(QPixmap(":/icons/speed.svg").scaled(100,100,Qt::KeepAspectRatio));
    rpmGauge->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    auto *rpmNum = new QLabel("2500 rpm");
    rpmNum->setStyleSheet("QLabel {font-size: 25pt;}");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [kmLeftNum, fuelCurrNum, rpmNum](){
        kmLeftNum->setText(QString::number(stuff.fuelToEnd) + " Km");
        fuelCurrNum->setText(QString::number(stuff.fuelNow) + " /100");
        rpmNum->setText(QString::number(stuff.rpms) + " rpm");
    });
    timer->start(50);

    horLay->setSpacing(0);
    horLay->addWidget(spaceLabel,0,0);
    horLay->addWidget(kmLeft,1,0,Qt::AlignCenter);
    horLay->addWidget(kmLeftNum,2,0,Qt::AlignCenter);

    horLay->addWidget(fuelCurr,1,1,Qt::AlignCenter);
    horLay->addWidget(fuelCurrNum,2,1,Qt::AlignCenter);

    horLay->addWidget(rpmGauge,1,3,Qt::AlignCenter);
    horLay->addWidget(rpmNum,2,3,Qt::AlignCenter);
    horLay->addWidget(spaceLabel,3,0);

    //layout->addWidget(label);
    layout->addLayout(horLay);


}
