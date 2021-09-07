#include <QElapsedTimer>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "app/arbiter.hpp"
#include "app/widgets/dialog.hpp"

#include "app/quick_views/combo.hpp"

ComboQuickView::ComboQuickView(Arbiter &arbiter)
    : QFrame()
    , QuickView(arbiter, "combo", this)
{
}

void ComboQuickView::init()
{
    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto volume = new QPushButton();
    volume->setFlat(true);
    this->arbiter.forge().iconize("volume_up", volume, 26);
    auto volume_dialog = new Dialog(this->arbiter, false, volume);
    volume_dialog->set_body(this->arbiter.forge().volume_slider(false));
    auto volume_timer = new QElapsedTimer();
    connect(volume, &QPushButton::pressed, [volume_timer]{ volume_timer->start(); });
    connect(volume, &QPushButton::released, [this, volume_timer, volume_dialog]{
        if (volume_timer->hasExpired(1000))
            this->arbiter.set_volume(0);
        else
            volume_dialog->open(2000);
    });
    auto volume_value = new QLabel(QString::number(this->arbiter.system().volume));
    volume_value->setFont(this->arbiter.forge().font(10));
    connect(&this->arbiter, &Arbiter::volume_changed, [volume_value](uint8_t volume){
        volume_value->setText(QString::number(volume));
    });


    auto dark_mode = new QPushButton();
    dark_mode->setFlat(true);
    this->arbiter.forge().iconize("dark_mode", dark_mode, 26);
    connect(dark_mode, &QPushButton::clicked, [this]{ this->arbiter.toggle_mode(); });

    layout->addWidget(volume, 1);
    layout->addWidget(volume_value, 7);
    layout->addStretch();
    layout->addWidget(dark_mode, 1);
}
