#include <fileref.h>
#include <math.h>
#include <tag.h>
#include <tpropertymap.h>
#include <BluezQt/PendingCall>
#include <QDirIterator>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMediaPlaylist>

#include "app/window.hpp"
#include "app/pages/media.hpp"
#include "canbus/psaserial.hpp"
#include "app/widgets/messages.hpp"


MediaPage::MediaPage(Arbiter &arbiter, QWidget *parent)
    : QTabWidget(parent)
    , Page(arbiter, "Media", "play_circle_outline", true, this)
{

}

void MediaPage::init()
{
    this->addTab(new RadioPlayerTab(this->arbiter, this), "Media");
    this->addTab(new CDPlayerTab(this->arbiter, this), "CD");
    this->addTab(new AuxPlayerTab(this->arbiter, this), "AUX");
    this->addTab(new USBPlayerTab(this->arbiter, this), "USB");
    //this->addTab(new LocalPlayerTab(this->arbiter, this), "Local");
    this->addTab(this->setting_menu(), "test");
    psaserial *pog = new psaserial(this->arbiter);
    pog->createDialog(this->arbiter, this->window());
    pog->contactSerial();
    this->volume = new VolumeSnackBar(arbiter);
    this->msg = new MessageSnackBar(arbiter);
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this](){
        if (oldMode != stuff.opMode){
            switch (stuff.opMode){
            case 1:
                this->setCurrentIndex(0);
                oldMode = stuff.opMode;
                break;
            case 2:
                this->setCurrentIndex(1);
                oldMode = stuff.opMode;
                break;
            case 4:
                this->setCurrentIndex(2);
                oldMode = stuff.opMode;
                break;
            case 6:
                this->setCurrentIndex(3);
                oldMode = stuff.opMode;
                break;
            }

        }
        if (oldVol != stuff.volume[1] && stuff.volume[0] == 0){
            volume->setVolume(stuff.volume[1]);
            oldVol = stuff.volume[1];
        }
        if (oldMsg != stuff.diagShow){
            msg->openMessage(stuff.diag, stuff.diagShow);
            oldMsg = stuff.diagShow;
        }
    });
    timer->start(100);


}

AuxPlayerTab::AuxPlayerTab(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    , arbiter(arbiter){

    auto *vLayout = new QVBoxLayout(this);
    auto *hLayout = new QHBoxLayout();

    auto *label = new QLabel("Playing From AUX");
    label->setStyleSheet("QLabel {font-size: 40pt;}");
    label->setAlignment(Qt::AlignHCenter);

    hLayout->addWidget(label, Qt::AlignHCenter);
    vLayout->addStretch(0);
    vLayout->addLayout(hLayout);
    vLayout->addStretch(0);

}

AuxPlayerTab::~AuxPlayerTab()
{

}

QWidget *MediaPage::setting_menu(){
    QWidget *widget = new QWidget(this);
    auto *layout = new QVBoxLayout(widget);


    QPushButton *settings_button = new QPushButton(widget);
    settings_button->setFlat(true);
    this->arbiter.forge().iconize("settings", settings_button, 24);

    QPushButton *change = new QPushButton(widget);
    change->setFlat(true);
    change->setText("Show");
    connect(change, &QPushButton::clicked, [](){stuff.diagShow = !stuff.diagShow;});

    QSpinBox *input = new QSpinBox();

    QPushButton *changeText = new QPushButton();
    changeText->setFlat(true);
    changeText->setText("Text Change");
    //connect(changeText, &QPushButton::clicked, [this, dialog](){dialog->open();});

    layout->addWidget(input);
    layout->addWidget(changeText);
    return widget;
}


RadioPlayerTab::RadioPlayerTab(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    , arbiter(arbiter)
    , config(Config::get_instance())
    , tuner(new Tuner(this->arbiter))
    //, loader()
{
    this->tuner->setValue(this->config->get_radio_station());
    auto layout = new QVBoxLayout(this);



    layout->addStretch(1);
    layout->addWidget(this->tuner_widget(), 4);
    layout->addWidget(this->controls_widget(), 4);
    layout->addStretch(2);
}

RadioPlayerTab::~RadioPlayerTab()
{
    this->loader.unload();
}


QWidget *RadioPlayerTab::tuner_widget()
{
    auto widget = new QWidget(this);
    auto layout = new QGridLayout(widget);
    layout->setSpacing(0);

    auto station = new QLabel("POG");
    station->setStyleSheet("QLabel {font-size: 40pt;}");

    auto freq = new QLabel("POG");
    freq->setStyleSheet("QLabel {font-size: 35pt;}");

    auto band = new QLabel("FM-1");
    band->setStyleSheet("QLabel {font-size: 25pt;}");
    //band->setFont(this->arbiter.forge().font(30, true));

    auto memory = new QLabel("1");
    memory->setStyleSheet("QLabel {font-size: 25pt;}");


    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this, station, freq, band, memory](){
        // This sets the memory text
        QString memText;
        if (stuff.memory == 0){
            memText = QString("Not in Memory");
        }else{
            memText = QString::number(stuff.memory);
        }
        this->config->set_radio_station(stuff.frequency.toDouble());
        station->setText(stuff.radioName);
        freq->setText(stuff.frequency);
        band->setText(stuff.band);
        memory->setText("Mem " + memText);
        tuner->setSliderPosition(int(stuff.frequency.toDouble() * 10));
    });
    timer->start(100);

    //layout->addStretch(2);
    layout->addWidget(station,0,0, Qt::AlignHCenter);
    layout->addWidget(band,1,0, Qt::AlignHCenter);
    layout->addWidget(freq,0,1, Qt::AlignHCenter);
    layout->addWidget(memory,1,1, Qt::AlignHCenter);

    return widget;
}

QWidget *RadioPlayerTab::controls_widget()
{
    auto widget = new QWidget();
    auto layout = new QHBoxLayout(widget);

    auto prev_station = new QPushButton();
    prev_station->setFlat(true);
    this->arbiter.forge().iconize("chevron_left", prev_station, 56);
    connect(prev_station, &QPushButton::clicked, [tuner = this->tuner]{
        tuner->setSliderPosition(tuner->sliderPosition() - 1);
    });

    auto next_station = new QPushButton();
    next_station->setFlat(true);
    this->arbiter.forge().iconize("chevron_right", next_station, 56);
    connect(next_station, &QPushButton::clicked, [tuner = this->tuner]{
        tuner->setSliderPosition(tuner->sliderPosition() + 1);
    });


    layout->addStretch(1);
    //layout->addWidget(prev_station);
    layout->addWidget(this->tuner, 4);
    //layout->addWidget(next_station);
    layout->addStretch(1);

    return widget;
}

CDPlayerTab::CDPlayerTab(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    , arbiter(arbiter)
{
    auto layout = new QGridLayout(this);

    layout->addWidget(this->track_widget());
    layout->addWidget(this->controls_widget());
}

QWidget *CDPlayerTab::track_widget()
{

    QWidget *widget = new QWidget(this);
    auto layout = new QGridLayout(widget);


    QLabel *artist = new QLabel("Artist");
    artist->setStyleSheet("QLabel {font-size: 40pt;}");


    QLabel *title = new QLabel("Title");
    title->setStyleSheet("QLabel {font-size: 45pt;}");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this, artist, title](){
        artist->setText(stuff.CDArtist);
        title->setText(stuff.CDTitle);
    });
    timer->start(1000);

    layout->addWidget(artist, 1, 0, Qt::AlignHCenter);
    layout->addWidget(title, 0, 0, Qt::AlignHCenter);
    return widget;
}

QWidget *CDPlayerTab::controls_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QLabel *valueRem = new QLabel("1:01");
    QTimer *timer = new QTimer(this);
    //Remake literally everything plz.
    connect(timer, &QTimer::timeout, [this, valueRem](){
        valueRem->setText(QString::number(stuff.CDTimeCurr));
    });
    timer->start(1000);

    layout->addWidget(valueRem, 3);
    layout->addStretch(1);

    return widget;
}


//USB Stuff

USBPlayerTab::USBPlayerTab(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    , arbiter(arbiter)
{
    auto layout = new QGridLayout(this);

    layout->addWidget(this->track_widget());
    layout->addWidget(this->controls_widget());
}

QWidget *USBPlayerTab::track_widget()
{

    QWidget *widget = new QWidget(this);
    auto layout = new QGridLayout(widget);


    QLabel *artist = new QLabel("Artist");
    artist->setStyleSheet("QLabel {font-size: 40pt;}");


    QLabel *title = new QLabel("Title");
    title->setStyleSheet("QLabel {font-size: 45pt;}");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this, artist, title](){
        artist->setText(stuff.USBArtist);
        title->setText(stuff.USBTitle);
    });
    timer->start(1000);

    layout->addWidget(artist, 1, 0, Qt::AlignHCenter);
    layout->addWidget(title, 0, 0, Qt::AlignHCenter);
    return widget;
}

QWidget *USBPlayerTab::controls_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);

    QSlider *slider = new QSlider(Qt::Orientation::Horizontal, widget);
    slider->setTracking(false);
    slider->setRange(0, 0);
    QLabel *valueAll = new QLabel("1:01");
    QLabel *valueRem = new QLabel("1:01");
    QTimer *timer = new QTimer(this);
    //Remake literally everything plz.
    connect(timer, &QTimer::timeout, [this, valueRem, slider](){
        valueRem->setText(QString::number(stuff.USBTimeCurr));
        slider->setRange(0, stuff.USBTimeAll);
        slider->setValue(50);
    });
    timer->start(1000);

    layout->addStretch(4);
    layout->addWidget(valueRem, 3);
    layout->addWidget(slider, 28);
    layout->addWidget(valueAll, 3);
    layout->addStretch(1);

    return widget;
}


