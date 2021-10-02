#include <QApplication>
#include <QKeyEvent>
#include "canbus/psaserial.hpp"

psaserial::psaserial(Arbiter &arbiter)
    :arbiter(arbiter)
{

}

void psaserial::createDialog(Arbiter &arbiter, QWidget *parent){
    audSettings = new auSettings(arbiter, parent);
    dialog = new Dialog(arbiter, true, parent);
    dialog->set_body(audSettings);
}

void psaserial::contactSerial(){
    arduino_available = false;
    arduino_portName = "";
    arduino = new QSerialPort;

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
            if (serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()) {

                if ((serialPortInfo.productIdentifier() == arduino_pid) && (serialPortInfo.vendorIdentifier() == arduino_vid)) {
                    arduino_available = true;
                    qInfo() << "VID: " << serialPortInfo.vendorIdentifier() << " PID: " << serialPortInfo.productIdentifier() << endl;
                    arduino_portName = serialPortInfo.portName();
                    qInfo() << "Port: " << arduino_portName << endl;
                }
            }
        }

    if(arduino_available){
        // open and configure the serialport
        arduino->setPortName(arduino_portName);
        arduino->open(QSerialPort::ReadOnly);
        arduino->setBaudRate(QSerialPort::Baud115200);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino->setFlowControl(QSerialPort::NoFlowControl);

        QObject::connect(arduino, &QSerialPort::readyRead, [&]() {
                readSerial();
            });

    }
}



void psaserial::readSerial(){
    /*
     * readyRead() doesn't guarantee that the entire message will be received all at once.
     * The message can arrive split into parts.  Need to buffer the serial data and then parse for the temperature value.
     *
     */
    char STX = 0x12;
    char ETX = 0x13;

    //Parse info Here
    if (arduino->canReadLine()){
        serialData = arduino->readAll();
        int z = 0;
        while (serialData[z] != '\n'){
            z++;
            //qInfo() << "Z: " << z << endl;
        }
        serialParsed.clear();
        for (int j = 0; j <= z; j++){
            serialParsed.append(serialData[j]);
        }
        //qInfo() << "DATA: " << serialParsed.toHex() << endl;

        quint16 frequen;
        quint16 autonomy;
        quint16 fuelUsage;
        frameId = serialParsed[2];
        switch (frameId) {
            case 0x01:
                stuff.volume[1] = int(serialParsed[3] & 0b00011111);
                stuff.volume[0] = int(serialParsed[3] & 0b11100000);
                qInfo() << "Open: " << stuff.volume[0] << "Vol: " << stuff.volume[1] << endl;
            break;
            case 0x04:
                radioName = QString();
                //Handle Radio Frames
                for (int j = 3; j < int(serialParsed[1] + 2); j++){
                    radioName +=  serialParsed[j];
                }
                stuff.radioName = radioName;
                break;
            case 0x05:
                stuff.frequency = QString();
                frequen = quint16(serialParsed[3]) << 8 | (quint16(serialParsed[4])&0xFF);
                stuff.frequency = QString::number(double(frequen) / 10, 'f', 1);
                break;
            case 0x06:
                switch (serialParsed[3]){
                    case 1:
                        stuff.band = "FM-1";
                        break;

                    case 2:
                        stuff.band = "FM-2";
                        break;

                    case 4:
                        stuff.band = "FM-AST";
                        break;

                    case 5:
                        stuff.band = "AM";
                        break;
                    }
                break;
            case 0x1A:
                //CD Artist
                //Uses same variable because why shouldn't I confuse myself?
                cdArt.clear();
                for (int j = 3; j < serialParsed[1]+2; j++){
                    cdArt +=  serialParsed[j];
                    //qInfo() << cdArt << endl;
                }
                stuff.CDArtist = cdArt;
                break;
            case 0x1B:
                //CD Title
                //Uses same variable because why shouldn't I confuse myself?
                cdTit.clear();
                for (int j = 3; j < z-1; j++){
                    cdTit +=  serialParsed[j];
                }
                stuff.CDTitle = cdTit;
                break;
            case 0x0E:
                //Trip Instant stuff
                autonomy = quint16(serialParsed[3]) << 8 | (quint16(serialParsed[4])&0xFF);
                fuelUsage = quint16(serialParsed[1]) << 8 | (quint16(serialParsed[2])&0xFF);

                if (serialParsed[3] == char(0xFF)){
                    stuff.fuelToEnd = -1;
                }else{
                    stuff.fuelToEnd = autonomy;
                }
                if (serialParsed[1] == char(0xFF)){
                    stuff.fuelNow = 0;
                }else{
                    stuff.fuelNow = fuelUsage / 10.0;
                }
                break;
            case 0x42:
                if (serialParsed[3] == char(0x01)){
                    //Dark pressed, send keystroke or change theme
                    auto FOCUSOBJ = QApplication::focusObject();
                    QKeyEvent  eve1 =  QKeyEvent(QEvent::KeyPress,Qt::Key_D,Qt::NoModifier,"D");
                    QCoreApplication::sendEvent(FOCUSOBJ, &eve1);
                }
                break;
             case 0x41:
                if (serialParsed[3] & 0x01){
                    //Mode pressed, send S keystroke
                    auto FOCUSOBJ = QApplication::focusObject();
                    QKeyEvent  eve1 =  QKeyEvent(QEvent::KeyPress,Qt::Key_S,Qt::NoModifier,"S");
                    QCoreApplication::sendEvent(FOCUSOBJ, &eve1);
                }
                break;
            case 0x08:
                if (serialParsed[0] & 0x01){
                    stuff.diagShow = true;
                }else{
                    stuff.diagShow = false;
                }
                //Diagnostic Info Frame Handling
                stuff.diag = serialParsed[1];
                break;
            case 0x03:
                if (stuff.opMode != serialParsed[3]){
                    stuff.opMode = serialParsed[3];
                }
                break;
            case 0x1C:
                if (stuff.memory != serialParsed[3]){
                    stuff.memory = serialParsed[3];
                }
                break;
            case 0x10:
            QString bal = QString("%1").arg(quint8(serialParsed[3]), 8, 2, QChar('0'));
            QString fad = QString("%1").arg(quint8(serialParsed[4]), 8, 2, QChar('0'));
            QString bass = QString("%1").arg(quint8(serialParsed[5]), 8, 2, QChar('0'));
            QString treble = QString("%1").arg(quint8(serialParsed[7]), 8, 2, QChar('0'));
            QString manyThings = QString("%1").arg(quint8(serialParsed[8]), 8, 2, QChar('0'));
            QString eq = QString("%1").arg(quint8(serialParsed[9]), 8, 2, QChar('0'));
            if (aSettings.isBass != (bass[0])){
                aSettings.isBass = bool(bass[0].digitValue());

            }
            if (aSettings.isTreble != treble[0]){
                aSettings.isTreble = bool(treble[0].digitValue());
            }
            if (aSettings.isBalance != bal[0]){
                aSettings.isBalance = bool(bal[0].digitValue());
            }
            if (aSettings.isFader != fad[0]){
                aSettings.isFader = bool(fad[0].digitValue());
            }
            if (aSettings.isLoud != manyThings[0]){
                aSettings.isLoud = bool(manyThings[0].digitValue());
            }
            if (aSettings.isAutoVol != manyThings[3]){
                aSettings.isAutoVol = bool(manyThings[3].digitValue());
            }
            if (aSettings.isEq != eq[1]){
                aSettings.isEq = bool(eq[1].digitValue());
            }
            aSettings.balance = bal.remove(0,1).toInt(nullptr, 2) - 63;
            aSettings.fade = fad.remove(0,1).toInt(nullptr, 2) - 63;
            aSettings.bass = bass.remove(0,1).toInt(nullptr, 2) - 63;
            aSettings.treble = treble.remove(0,1).toInt(nullptr, 2) - 63;
            aSettings.loudness = bool(manyThings[1].digitValue());

            aSettings.autoVol = bool(manyThings[7].digitValue());
            aSettings.equalizer = eq.remove(0, 3).toInt(nullptr, 2);
            qInfo() << "EQ Thing: " << aSettings.equalizer <<endl;

            addAudioSettings();

            //qInfo() << "Is Bass: " << QString("%1").arg(quint8(serialParsed[3]), 8, 2, QChar('0'))[0] << endl;

                break;
        }
    }
}

void psaserial::addAudioSettings(){
    if (aSettings.loudness){
        audSettings->loudstate->setText("On");
    }else{
        audSettings->loudstate->setText("Off");
    }
    if (aSettings.autoVol){
        audSettings->autoVolState->setText("On");
    }else{
        audSettings->autoVolState->setText("Off");
    }
    if (aSettings.equalizer == 3){
        audSettings->eqState->setText("None");
    }else if (aSettings.equalizer == 7){
        audSettings->eqState->setText("Classical");
    }else if (aSettings.equalizer == 11){
        audSettings->eqState->setText("Jazz/Blues");
    }else if (aSettings.equalizer == 15){
        audSettings->eqState->setText("Pop/Rock");
    }else if (aSettings.equalizer == 19){
        audSettings->eqState->setText("Vocal");
    }else if (aSettings.equalizer == 23){
        audSettings->eqState->setText("Techno");
    }
    if (aSettings.isBass){
        audSettings->bassSlide->setEnabled(true);
    }else{
        audSettings->bassSlide->setEnabled(false);
    }
    if (aSettings.isTreble){
        audSettings->trebSlide->setEnabled(true);
    }else{
        audSettings->trebSlide->setEnabled(false);
    }
    if (aSettings.isBalance){
        audSettings->balanceSlide->setEnabled(true);
    }else{
        audSettings->balanceSlide->setEnabled(false);
    }
    if (aSettings.isFader){
        audSettings->faderSlide->setEnabled(true);
    }else{
        audSettings->faderSlide->setEnabled(false);
    }
    audSettings->bassSlide->setValue(aSettings.bass);
    audSettings->trebSlide->setValue(aSettings.treble);
    audSettings->balanceSlide->setValue(aSettings.balance);
    audSettings->faderSlide ->setValue(aSettings.fade);
    bool test = aSettings.isAutoVol || aSettings.isLoud || aSettings.isEq || aSettings.isFader || aSettings.isBass || aSettings.isTreble || aSettings.isBalance;
    if (test && !isOpen){
        dialog->open();
        isOpen = true;
    }else if (isOpen && !test){
        dialog->close();
        isOpen = false;
    }
}

auSettings::auSettings(Arbiter &arbiter, QWidget *parent)
    : QWidget(parent)
    , arbiter(arbiter)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    //layout->setContentsMargins(0,0,0,0);
    QLabel *text = new QLabel("Audio Settings");
    layout->addWidget(text);
    layout->addWidget(Session::Forge::br(), 1);
    layout->addLayout(this->settings_widget());

}

QSize auSettings::sizeHint() const
{
    int label_width = QFontMetrics(this->font()).averageCharWidth() * 20;
    return QSize(800, 500);
}

QWidget *auSettings::body_widget(){
    QWidget *widget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    //layout->setContentsMargins(0,0,0,0);
    QLabel *text = new QLabel("Audio Settings");
    layout->addWidget(text);
    layout->addWidget(Session::Forge::br(), 1);
    layout->addLayout(this->settings_widget());

    return widget;
}

QLayout *auSettings::settings_widget(){
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

QLayout *auSettings::loud_widget(){
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *text = new QLabel("Loudness");
    loudstate = new QLabel(QString::number(aSettings.isLoud));
    loudstate->setAlignment(Qt::AlignHCenter);
    text->setAlignment(Qt::AlignHCenter);
    text->setStyleSheet("QLabel::hover{background-color: palette(base); padding-right: 10px; border-radius: 10px;}");
    layout->addWidget(text);
    layout->addWidget(loudstate);

    return layout;
}

QLayout *auSettings::autoSound_widget(){
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *text = new QLabel("Auto Volume");
    autoVolState = new QLabel("On");
    autoVolState->setAlignment(Qt::AlignHCenter);
    text->setAlignment(Qt::AlignHCenter);

    layout->addWidget(text);
    layout->addWidget(autoVolState);

    return layout;
}

QLayout *auSettings::eq_widget(){
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *text = new QLabel("Eq Preset");
    eqState = new QLabel("Classic");
    eqState->setAlignment(Qt::AlignHCenter);
    text->setAlignment(Qt::AlignHCenter);

    layout->addWidget(text);
    layout->addWidget(eqState);

    return layout;
}

QLayout *auSettings::bass_widget(){
    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *layout2 = new QHBoxLayout();
    QHBoxLayout *layout3 = new QHBoxLayout();
    QLabel *text = new QLabel("Bass");
    QLabel *numbMin = new QLabel("-9");
    QLabel *numbMax = new QLabel("9");
    bassSlide = new QSlider(Qt::Orientation::Horizontal);
    bassSlide->setRange(-9, 9);
    bassSlide->setValue(0);
    bassSlide->setEnabled(false);
    text->setAlignment(Qt::AlignBottom);
    text->setAlignment(Qt::AlignHCenter);
    layout2->addWidget(text);
    layout3->addWidget(numbMin,1, Qt::AlignRight);
    layout3->addWidget(bassSlide,5);
    layout3->addWidget(numbMax,1, Qt::AlignLeft);
    layout->addLayout(layout2);
    layout->addLayout(layout3);

    return layout;
}

QLayout *auSettings::treble_widget(){
    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *layout2 = new QHBoxLayout();
    QHBoxLayout *layout3 = new QHBoxLayout();
    QLabel *text = new QLabel("Treble");
    QLabel *numbMin = new QLabel("-9");
    QLabel *numbMax = new QLabel("9");
    trebSlide = new QSlider(Qt::Orientation::Horizontal);
    trebSlide->setRange(-9, 9);
    trebSlide->setValue(0);
    text->setAlignment(Qt::AlignBottom);
    text->setAlignment(Qt::AlignHCenter);
    layout2->addWidget(text);
    layout3->addWidget(numbMin,1, Qt::AlignRight);
    layout3->addWidget(trebSlide,5);
    layout3->addWidget(numbMax,1, Qt::AlignLeft);
    layout->addLayout(layout2);
    layout->addLayout(layout3);

    return layout;
}

QLayout *auSettings::bal_widget(){
    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *layout2 = new QHBoxLayout();
    QHBoxLayout *layout3 = new QHBoxLayout();
    QLabel *text = new QLabel("Balance");
    QLabel *numbMin = new QLabel("L");
    QLabel *numbMax = new QLabel("R");
    balanceSlide = new QSlider(Qt::Orientation::Horizontal);
    balanceSlide->setRange(-9, 9);
    balanceSlide->setValue(0);
    text->setAlignment(Qt::AlignBottom);
    text->setAlignment(Qt::AlignHCenter);
    layout2->addWidget(text);
    layout3->addWidget(numbMin,1, Qt::AlignRight);
    layout3->addWidget(balanceSlide,5);
    layout3->addWidget(numbMax,1, Qt::AlignLeft);
    layout->addLayout(layout2);
    layout->addLayout(layout3);

    return layout;
}

QLayout *auSettings::fad_widget(){
    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *layout2 = new QHBoxLayout();
    QHBoxLayout *layout3 = new QHBoxLayout();
    QLabel *numbMin = new QLabel("R");
    QLabel *numbMax = new QLabel("F");
    faderSlide = new QSlider(Qt::Orientation::Horizontal);
    faderSlide->setRange(-9, 9);
    faderSlide->setValue(0);
    layout3->addWidget(numbMin,1, Qt::AlignRight);
    layout3->addWidget(faderSlide,5);
    layout3->addWidget(numbMax,1, Qt::AlignLeft);
    layout->addLayout(layout2);
    layout->addLayout(layout3);

    return layout;
}


radioData stuff = {
    {0, 0}, //Volume
    "Station Na", //Radio Name
    "90.0", //Radio Freq
    "FM-1", //Radio Band
    1, //Radio Memory
    "No Artist Name", //CD Artist
    "No Track Name", //CD Title
    101, //CD Current Time
    301, //CD Time
    "No Artist Name", //USB Artist
    "No Track Name", //USB Title
    101, //USB Current Time
    301, //USB Time
    115, //Fuel To Ded
    6.5, //Fuel Now
    2500, //RPM
    false, //Show Diag
    0x00, //Diag Message Code
    0 //Operation Mode (Radio, Cd, USB, etc)
};

audioSettings aSettings = {
    0, //Bass
    0, //Treble
    0, //Balance
    0, //Fader
    2, //EQ
    false, //Loud
    false, //Auto Vol
    false, //Bass
    false, //Treble
    false, //Bal
    false, //Fade
    false, //Eq
    false, //isLoud
    false //isAutoVol
};


