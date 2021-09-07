#include "canbus/psaserial.hpp"

psaserial::psaserial()
{
    contactSerial();
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
    serialData = arduino->readAll();
    for (uint32_t i = 0; i < (serialData.size()); ++i){
        //Unframe the frame

        if (serialData[i] == STX){
            //This indicates start of frame
            if (serialData[int(serialData[i+1]) + 2] == ETX){
                int z = 0;
                serialParsed = QByteArray();
                for (int j = i; j < int(serialData[i+1])+3; j++){
                    serialParsed[z] = serialData[j];
                    z++;
                }

                frameId = serialParsed[2];
                if (frameId == 0x01){
                    //qInfo() << "Volume: " << int(serialData[3]) << endl;
                    stuff.volume = int(serialParsed[3]);
                }else if (frameId == 0x04){
                    radioName = QString();
                    //Handle Radio Frames
                    for (int j = 3; j < int(serialParsed[1] + 2); j++){
                        radioName +=  serialParsed[j];
                    }
                    stuff.radioName = radioName;
                    //emit this->valueChanged();

                }else if (frameId == 0x05){
                    stuff.frequency = QString();
                    quint16 frequen = quint16(serialParsed[3]) << 8 | (quint16(serialParsed[4])&0xFF);
                    stuff.frequency = QString::number(double(frequen) / 10, 'f', 1);
                }else if (frameId == 0x06){
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

                }else if (frameId == 0x12){
                    //CD Artist
                    //Uses same variable because why shouldn't I confuse myself?
                    radioName = QString();
                    for (int j = 3; j < int(serialParsed[1] + 2); j++){
                        radioName +=  serialParsed[j];
                    }
                    stuff.CDArtist = radioName;
                }else if (frameId == 0x13){
                    //CD Title
                    //Uses same variable because why shouldn't I confuse myself?
                    radioName = QString();
                    for (int j = 3; j < int(serialParsed[1] + 2); j++){
                        radioName +=  serialParsed[j];
                    }
                    stuff.CDTitle = radioName;
                }

            }
            serialData.remove(i, int(serialData[i+1])+3);
            i--;
        }
    }



}

radioData stuff = {0,"ERA","87.0","FM-1", "1", "Artist", "kalispera eimai terma kariolis", 101, 301, "Artist", "Titlos tou USB", 101, 301};


