#pragma once
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include "app/arbiter.hpp"

class auSettings : public QWidget {
    Q_OBJECT
   public:
    auSettings(Arbiter &arbiter, QWidget *parent = nullptr);
    Arbiter &arbiter;
    QWidget *body_widget();

    QLabel *loudstate;
    QLabel *autoVolState;
    QLabel *eqState;
    QSlider *bassSlide;
    QSlider *trebSlide;
    QSlider *balanceSlide;
    QSlider *faderSlide;

   protected:
       QSize sizeHint() const override;
   private:
    int eqVal;

    QLayout *settings_widget();
    QLayout *bass_widget();
    QLayout *treble_widget();
    QLayout *bal_widget();
    QLayout *fad_widget();
    QLayout *eq_widget();
    QLayout *loud_widget();
    QLayout *autoSound_widget();

};

class psaserial : public QMessageBox
{
    Q_OBJECT
    public:
        psaserial(Arbiter &arbiter);
        //~psaserial();
        void contactSerial();
        int volume;
        QString radioName;
        QString getradioName();
        void setRadioName(QString value);
        Arbiter &arbiter;
        auSettings *audSettings;
        Dialog *dialog;
        void createDialog(Arbiter &arbiter, QWidget *parent = nullptr);
        void addAudioSettings();
        bool isOpen = false;
    private:
        bool arduino_available;

        QString arduino_portName;
        //Set those correctly
        static const quint16 arduino_pid = 29987;
        static const quint16 arduino_vid = 6790;
        int frameId;
        QString cdTit;
        QString cdArt;
        QSerialPort *arduino;
        QByteArray serialData;
        QVector<QByteArray> serialParsed;
        QByteArray serialBuffer;
        QString parsed_data;
    private slots:
        void readSerial();
    signals:
        void valueChanged();
};



struct radioData{
    int volume[2];
    QString radioName;
    QString frequency;
    QString band;
    int memory;
    QString CDArtist;
    QString CDTitle;
    int CDTimeCurr;
    int CDTimeAll;
    QString USBArtist;
    QString USBTitle;
    int USBTimeCurr;
    int USBTimeAll;
    int fuelToEnd;
    float fuelNow;
    int rpms;
    bool diagShow;
    int diag;
    int opMode;
};

struct audioSettings{
    int bass;
    int treble;
    int balance;
    int fade;
    int equalizer;
    bool loudness;
    bool autoVol;
    bool isBass;
    bool isTreble;
    bool isBalance;
    bool isFader;
    bool isEq;
    bool isLoud;
    bool isAutoVol;
};




extern radioData stuff;
extern audioSettings aSettings;
