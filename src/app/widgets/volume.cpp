#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QString>
#include "app/widgets/volume.hpp"

VolumeSnackBar::VolumeSnackBar(Arbiter &arbiter)
    : SnackBar(arbiter)
{
    this->setFocusPolicy(Qt::NoFocus);
    this->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    this->set_body(this->body_widget());
}

QWidget *VolumeSnackBar::body_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    //this->vol = new QLabel(widget);
    this->slider = new QSlider(Qt::Orientation::Horizontal, widget);
    this-> slider->setRange(0,30);
    this->slider->setValue(15);
    //this->vol->setAlignment(Qt::AlignCenter);
    layout->addWidget(this->slider, 4);
    //layout->addWidget(this->vol);


    return widget;
}

void VolumeSnackBar::setVolume(int volume)
{
    //this->vol->setText(QString::number(volume));
    this->slider->setValue(volume);
    this->open(1500);
}

void VolumeSnackBar::openVolume(int volume)
{
    //this->vol->setText(QString::number(volume));
    this->slider->setValue(volume);
    this->open(1000);
}
