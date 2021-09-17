#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QString>
#include "app/widgets/messages.hpp"

MessageSnackBar::MessageSnackBar(Arbiter &arbiter)
    : SnackBar(arbiter)
{
    this->setFocusPolicy(Qt::NoFocus);
    this->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    this->set_body(this->body_widget());
}

QWidget *MessageSnackBar::body_widget()
{
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    this->msg = new QLabel("All OK");
    this->msg->setAlignment(Qt::AlignCenter);
    layout->addWidget(msg);


    return widget;
}

void MessageSnackBar::setMessage(int message)
{
    QString tempText;
    switch (message){
    case 0x00:
        tempText = "Diagnosis OK";
        break;
    case 0x01:
        tempText = "Engine Temperature Too High";
        break;
    case 0x03:
        tempText = "Cooland Circuit Level Too Low";
        break;
    case 0x04:
        tempText = "Check Engine Oil Level";
        break;
    case 0x05:
        tempText = "Engine Oil Pressure Too Low";
        break;
    case 0x08:
        tempText = "Brake System Faulty";
        break;
    case 0x0B:
        tempText = "Door Open";
        break;
    case 0x13:
        tempText = "Power Steering Faulty";
        break;
    case 0x61:
        tempText = "Handbrake On";
        break;
    case 0x67:
        tempText = "Break Pads Worn";
        break;
    case 0x6A:
        tempText = "ABS System Faulty";
        break;
    case 0xE0:
        tempText = "Fuel Level Too Low";
        break;
    case 0xD8:
        tempText = "Risk Of Ice";
        break;
    }
    this->msg->setText(tempText);

}

void MessageSnackBar::openMessage(int message, bool show)
{
    this->setMessage(message);
    if (show){
        this->open();
    }else{
        this->close();
    }

}
