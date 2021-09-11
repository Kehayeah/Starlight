#include <fileref.h>
#include <math.h>
#include <tag.h>
#include <tpropertymap.h>
#include <BluezQt/PendingCall>
#include <QDirIterator>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMediaPlaylist>

#include "app/window.hpp"
#include "app/pages/media.hpp"
#include "canbus/psaserial.hpp"



MediaPage::MediaPage(Arbiter &arbiter, QWidget *parent)
    : QTabWidget(parent)
    , Page(arbiter, "Media", "play_circle_outline", true, this)
{

}

void MediaPage::init()
{
    this->addTab(new RadioPlayerTab(this->arbiter, this), "Media");
    this->addTab(new CDPlayerTab(this->arbiter, this), "CD");
    this->addTab(new USBPlayerTab(this->arbiter, this), "USB");
    //this->addTab(new LocalPlayerTab(this->arbiter, this), "Local");
    this->addTab(this->setting_menu(), "test");

}

QWidget *MediaPage::setting_menu(){
    QWidget *widget = new QWidget(this);
    Dialog *dialog = new Dialog(this->arbiter, true, this->window());
    dialog->set_body(new MediaPage::Settings(this->arbiter, this));
    QPushButton *settings_button = new QPushButton(widget);
    settings_button->setFlat(true);
    this->arbiter.forge().iconize("settings", settings_button, 24);
    connect(settings_button, &QPushButton::clicked, [dialog]() { dialog->open(); });

    return widget;
}

MediaPage::Settings::Settings(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    , arbiter(arbiter)
{

    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *text = new QLabel("Audio Settings");
    layout->addWidget(text);
    layout->addWidget(Session::Forge::br(), 1);
    layout->addLayout(this->settings_widget());
}

QSize MediaPage::Settings::sizeHint() const
{
    int label_width = QFontMetrics(this->font()).averageCharWidth() * 20;
    return QSize(800, 500);
}


QLayout *MediaPage::Settings::settings_widget(){
    QVBoxLayout *layout = new QVBoxLayout();
    QVBoxLayout *bassTreb = new QVBoxLayout();
    QGridLayout *topRow = new QGridLayout();
    QGridLayout *botRow = new QGridLayout();

    bassTreb->addLayout(this->bass_widget(), 1);
    bassTreb->addLayout(this->treble_widget(), 1);
    topRow->addLayout(bassTreb, 0, 0, Qt::AlignCenter);
    topRow->addLayout(this->loud_widget(), 0, 1, Qt::AlignCenter);
    botRow->addLayout(this->autoSound_widget(), 0, 0, Qt::AlignCenter);
    botRow->addLayout(this->eq_widget(), 0, 1, Qt::AlignCenter);
    layout->addLayout(topRow);
    layout->addLayout(this->bal_widget(), 1);
    layout->addLayout(this->fad_widget(), 1);
    layout->addLayout(botRow);
    return layout;

}

QLayout *MediaPage::Settings::loud_widget(){
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *text = new QLabel("Loudness");
    QLabel *state = new QLabel("On");
    state->setAlignment(Qt::AlignHCenter);
    text->setAlignment(Qt::AlignHCenter);
    text->setStyleSheet("QLabel::hover{background-color: palette(base); padding-right: 10px; border-radius: 10px;}");
    layout->addWidget(text);
    layout->addWidget(state);

    return layout;
}

QLayout *MediaPage::Settings::autoSound_widget(){
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *text = new QLabel("Auto Volume");
    QLabel *state = new QLabel("On");
    state->setAlignment(Qt::AlignHCenter);
    text->setAlignment(Qt::AlignHCenter);

    layout->addWidget(text);
    layout->addWidget(state);

    return layout;
}

QLayout *MediaPage::Settings::eq_widget(){
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *text = new QLabel("Eq Preset");
    QLabel *state = new QLabel("Classic");
    state->setAlignment(Qt::AlignHCenter);
    text->setAlignment(Qt::AlignHCenter);

    layout->addWidget(text);
    layout->addWidget(state);

    return layout;
}

QLayout *MediaPage::Settings::bass_widget(){
    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *layout2 = new QHBoxLayout();
    QHBoxLayout *layout3 = new QHBoxLayout();
    QLabel *text = new QLabel("Bass");
    QLabel *numbMin = new QLabel("-9");
    QLabel *numbMax = new QLabel("9");
    QSlider *slide = new QSlider(Qt::Orientation::Horizontal);
    slide->setRange(-9, 9);
    slide->setValue(0);
    slide->setEnabled(false);
    text->setAlignment(Qt::AlignBottom);
    text->setAlignment(Qt::AlignHCenter);
    layout2->addWidget(text);
    layout3->addWidget(numbMin,1, Qt::AlignRight);
    layout3->addWidget(slide,5);
    layout3->addWidget(numbMax,1, Qt::AlignLeft);
    layout->addLayout(layout2);
    layout->addLayout(layout3);

    return layout;
}

QLayout *MediaPage::Settings::treble_widget(){
    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *layout2 = new QHBoxLayout();
    QHBoxLayout *layout3 = new QHBoxLayout();
    QLabel *text = new QLabel("Treble");
    QLabel *numbMin = new QLabel("-9");
    QLabel *numbMax = new QLabel("9");
    QSlider *slide = new QSlider(Qt::Orientation::Horizontal);
    slide->setRange(-9, 9);
    slide->setValue(0);
    text->setAlignment(Qt::AlignBottom);
    text->setAlignment(Qt::AlignHCenter);
    layout2->addWidget(text);
    layout3->addWidget(numbMin,1, Qt::AlignRight);
    layout3->addWidget(slide,5);
    layout3->addWidget(numbMax,1, Qt::AlignLeft);
    layout->addLayout(layout2);
    layout->addLayout(layout3);

    return layout;
}

QLayout *MediaPage::Settings::bal_widget(){
    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *layout2 = new QHBoxLayout();
    QHBoxLayout *layout3 = new QHBoxLayout();
    QLabel *text = new QLabel("Balance");
    QLabel *numbMin = new QLabel("L");
    QLabel *numbMax = new QLabel("R");
    QSlider *slide = new QSlider(Qt::Orientation::Horizontal);
    slide->setRange(-9, 9);
    slide->setValue(0);
    text->setAlignment(Qt::AlignBottom);
    text->setAlignment(Qt::AlignHCenter);
    layout2->addWidget(text);
    layout3->addWidget(numbMin,1, Qt::AlignRight);
    layout3->addWidget(slide,5);
    layout3->addWidget(numbMax,1, Qt::AlignLeft);
    layout->addLayout(layout2);
    layout->addLayout(layout3);

    return layout;
}

QLayout *MediaPage::Settings::fad_widget(){
    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *layout2 = new QHBoxLayout();
    QHBoxLayout *layout3 = new QHBoxLayout();
    QLabel *numbMin = new QLabel("R");
    QLabel *numbMax = new QLabel("F");
    QSlider *slide = new QSlider(Qt::Orientation::Horizontal);
    slide->setRange(-9, 9);
    slide->setValue(0);
    layout3->addWidget(numbMin,1, Qt::AlignRight);
    layout3->addWidget(slide,5);
    layout3->addWidget(numbMax,1, Qt::AlignLeft);
    layout->addLayout(layout2);
    layout->addLayout(layout3);

    return layout;
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
        this->config->set_radio_station(stuff.frequency.toDouble());
        station->setText(stuff.radioName);
        freq->setText(stuff.frequency);
        band->setText(stuff.band);
        memory->setText("Mem " + stuff.memory);
        tuner->setSliderPosition(int(stuff.frequency.toDouble() * 10));
    });
    timer->start(50);

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
    auto layout = new QGridLayout();
    QVBoxLayout *layout2 = new QVBoxLayout(widget);


    QLabel *artist = new QLabel("Artist");
    artist->setStyleSheet("QLabel {font-size: 35pt;}");
    artist->setAlignment(Qt::AlignHCenter);


    QLabel *title = new QLabel("Title");
    title->setStyleSheet("QLabel {font-size: 40pt;}");
    title->setAlignment(Qt::AlignHCenter);

    QLabel *trackNum = new QLabel("Track --/---");
    trackNum->setStyleSheet("QLabel {font-size: 30pt;}");

    QLabel *cdType = new QLabel("Audio CD");
    cdType->setStyleSheet("QLabel {font-size: 30pt;}");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this, artist, title](){
        artist->setText(stuff.CDArtist);
        title->setText(stuff.CDTitle);
    });
    timer->start(1000);

    layout2->addWidget(title);
    layout2->addWidget(artist);
    layout->addWidget(trackNum, 0, 0, Qt::AlignHCenter);
    layout->addWidget(cdType, 0, 1, Qt::AlignHCenter);
    layout2->addLayout(layout);
    return widget;
}

QWidget *CDPlayerTab::controls_widget()
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
        valueRem->setText(QString::number(stuff.CDTimeCurr));
        slider->setRange(0, stuff.CDTimeAll);
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
