#pragma once
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>



class psaserial : public QMessageBox
{
    Q_OBJECT
    public:
        psaserial();
        //~psaserial();
        void contactSerial();
        int volume;
        QString radioName;
        QString getradioName();
        void setRadioName(QString value);

    private:
        bool arduino_available;

        QString arduino_portName;
        //Set those correctly
        static const quint16 arduino_pid = 29987;
        static const quint16 arduino_vid = 6790;
        int frameId;
        QSerialPort *arduino;
        QByteArray serialData;
        QByteArray serialParsed;
        QString serialBuffer;
        QString parsed_data;
    private slots:
        void readSerial();
    signals:
        void valueChanged();
};

struct radioData{
    int volume;
    QString radioName;
    QString frequency;
    QString band;
    QString memory;
    QString CDArtist;
    QString CDTitle;
    int CDTimeCurr;
    int CDTimeAll;
    QString USBArtist;
    QString USBTitle;
    int USBTimeCurr;
    int USBTimeAll;
};

extern radioData stuff;
